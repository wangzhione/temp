
-- 是否锁 & 只能释放属于自己的锁
if redis.call("GET", KEYS[1]) == ARGV[1] then
    -- 执行成功返回 "1"
    return redis.call("DEL", KEYS[1])
else
    return 0
end