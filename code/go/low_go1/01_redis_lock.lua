--[[

SET key value [EX seconds] [PX milliseconds] [NX | XX]

- EX seconds : 设置键的过期时间为 second 秒. SET key value EX second 效果等同于 SETEX key second value

- PX milliseconds : 设置键的过期时间为 milliseconds 毫秒. SET key value PX millisecond 效果等同于 PSETEX key millisecond value

- NX : 只有键不存在时, 才对键进行设置操作. SET key value NX 效果等同于 SETNX key value

- XX : 只在键已经存在时, 才对键进行设置操作

--]]

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
