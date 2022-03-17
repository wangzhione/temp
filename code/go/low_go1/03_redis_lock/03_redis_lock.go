package main

import (
	"context"
	"log"
	"math/rand"
	"strconv"
	"sync/atomic"
	"time"

	"github.com/go-redis/redis/v8"
)

// build:
// go run 03_redis_lock.go
//
func main() {
	str := randNStr(0)
	println(&str, str, str == "")

	println(randNStr(valuen))
}

const (
	lockLua = `
	-- KEYS[1] : 锁 key
	-- ARGV[1] : 锁 value , 随机字符串
	-- ARGV[2] : 过期时间

	-- 判断锁 key 持有的 value 是否等于传入的 value
	-- 如果相等说明是再次获取锁并更新获取时间, 防止重入时过期
	-- 这里说明是 "可重入锁"
	if redis.call("GET", KEYS[1]) == ARGV[1] then
		-- 设置
		redis.call("SET", KEYS[1], ARGV[1], "PX", ARGV[2])
		return "OK"
	else
		-- 锁 key 的 value 不等于传入的 value 则说明是第一次获取锁
		-- SET key value NX PX timeout : 当 key 不存在时才设置 key 的值
		-- 设置成功会自动返回 "OK", 设置失败返回 "NULL Bulk Reply"
		-- 为什么这里需要加 "NX" 呢, 因为需要防止把别人的锁给覆盖了 
		return redis.call("SET", KEYS[1], ARGV[1], "NX", "PX", ARGV[2])
	end
	`

	unlockLua = `
	-- 是否锁 & 只能释放属于自己的锁
	if redis.call("GET", KEYS[1]) == ARGV[1] then
		-- 执行成功返回 "1"
		return redis.call("DEL", KEYS[1])
	else
		return 0
	end
	`

	// value 长度, randNStr valuen -> value
	valuen = 16
	// key 默认超时时间(毫秒), 防止死锁
	keyTimeout = 500
)

// RedisLock a simple redis try lock
type RedisLock struct {
	ctx    context.Context // context 上下文
	client *redis.Client   // go-redis 客户端
	expire uint32          // 超时的毫秒时间
	key    string          // 锁 key
	value  string          // 锁 value
}

func NewRedisLock(ctx context.Context, client *redis.Client, key string) *RedisLock {
	return &RedisLock{
		ctx:    ctx,
		client: client,
		key:    key,
		value:  randNStr(valuen),
	}
}

// SetExpire set expire millisecond
// need Acquire() 之前调用 SetExpire(), 否则默认 keyTimeout 500ms 释放
func (lock *RedisLock) SetExpire(expire uint32) {
	atomic.StoreUint32(&lock.expire, expire)
}

func (lock *RedisLock) Acquire() (bool, error) {
	// 获取过期时间
	millisecond := atomic.LoadUint32(&lock.expire)
	millisecond += keyTimeout
	milliseconds := strconv.FormatInt(int64(millisecond), 10)

	// 默认锁过期时间为 500ms, 防止死锁
	response, err := lock.client.Eval(lock.ctx,
		lockLua, []string{lock.key}, []string{lock.value, milliseconds}).Result()
	if err == redis.Nil {
		return false, nil
	}
	if err != nil {
		log.Printf("Acquire error = %+v, key = %s, expire = %d, value = %s, response=%+v\n",
			err, lock.key, lock.expire, lock.value, response)
		return false, err
	}
	if response == nil {
		return false, nil
	}

	res, ok := response.(string)
	if !ok || res != "OK" {
		log.Printf("Acquire Unknown error key = %s, expire = %d, value = %s, response=%+v\n",
			lock.key, lock.expire, lock.value, response)
		return false, nil
	}

	return true, nil
}

// Release release the lock 释放锁
func (lock *RedisLock) Release() (bool, error) {
	response, err := lock.client.Eval(lock.ctx,
		unlockLua, []string{lock.key}, []string{lock.value}).Result()
	if err == redis.Nil {
		return false, nil
	}
	if err != nil {
		log.Printf("Release error = %+v, key = %s, expire = %d, value = %s, response=%+v\n",
			err, lock.key, lock.expire, lock.value, response)
		return false, err
	}

	res, ok := response.(int64)
	if !ok {
		log.Printf("Release convert error key = %s, expire = %d, value = %s, response=%+v\n",
			lock.key, lock.expire, lock.value, response)
		return false, nil
	}
	return res == 1, nil
}

func init() {
	rand.Seed(time.Now().UnixNano())
}

const (
	letters = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
	lettern = len(letters)
)

func randNStr(n int) string {
	if n <= 0 {
		return ""
	}

	b := make([]byte, n)
	for i := range b {
		b[i] = letters[rand.Intn(lettern)]
	}
	return string(b)
}
