# MySQL 实操

```SQL
USE test;

CREATE TABLE IF NOT EXISTS transaction_test (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY COMMENT '物理自增主键',
    update_time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
    create_time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    context VARCHAR(255) NOT NULL DEFAULT ''
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT = 'transaction 操作表';

TRUNCATE transaction_test;
DROP TABLE transaction_test;

-- 插入几条数据
INSERT INTO transaction_test(context) VALUES ("8");
INSERT INTO transaction_test(context) VALUES ("4");
INSERT INTO transaction_test(context) VALUES ("9"), ("6");

SELECT * FROM transaction_test;
```

```SQL
MariaDB [test]> SELECT * FROM transaction_test;
+----+---------------------+---------------------+---------+
| id | update_time         | create_time         | context |
+----+---------------------+---------------------+---------+
|  1 | 2022-03-29 15:45:56 | 2022-03-29 15:45:56 | 8       |
|  2 | 2022-03-29 15:45:57 | 2022-03-29 15:45:57 | 4       |
|  3 | 2022-03-29 15:47:53 | 2022-03-29 15:47:53 | 9       |
|  4 | 2022-03-29 15:47:53 | 2022-03-29 15:47:53 | 6       |
+----+---------------------+---------------------+---------+
4 rows in set (0.01 sec)

```

我们开启两个事务, 一个更新, 一个删除

```SQL
-- 先查看事务

events_transactions_current，默认记录每个线程最近的一个事务信息

events_transactions_history，默认记录每个线程最近的十个事务信息

events_transactions_history_long ，默认记录每个线程最近的10000个事务信息

select * from performance_schema.setup_consumers where name like 'events_transactions%';

MariaDB [test]> show variables like '%tx_isolation%';
+---------------+-----------------+
| Variable_name | Value           |
+---------------+-----------------+
| tx_isolation  | REPEATABLE-READ |
+---------------+-----------------+
1 row in set (0.01 sec)

MariaDB [test]> SELECT @@global.tx_isolation;
+-----------------------+
| @@global.tx_isolation |
+-----------------------+
| REPEATABLE-READ       |
+-----------------------+
1 row in set (0.00 sec)

MariaDB [test]> SELECT @@session.tx_isolation;
+------------------------+
| @@session.tx_isolation |
+------------------------+
| REPEATABLE-READ        |
+------------------------+
1 row in set (0.00 sec)

-- 修改事务
-- http://c.biancheng.net/view/7266.html
```

https://cloud.tencent.com/developer/article/1401617

```SQL
select * from information_schema.innodb_trx\G

MariaDB [test]> SHOW VARIABLES LIKE 'autocommit';
ERROR 2006 (HY000): MySQL server has gone away
No connection. Trying to reconnect...
Connection id:    41
Current database: test

+---------------+-------+
| Variable_name | Value |
+---------------+-------+
| autocommit    | ON    |
+---------------+-------+
1 row in set (0.01 sec)

SET autocommit = 0|1|ON|OFF;

MariaDB [test]> SHOW GLOBAL VARIABLES LIKE 'innodb_lock_wait_timeout';
+--------------------------+-------+
| Variable_name            | Value |
+--------------------------+-------+
| innodb_lock_wait_timeout | 50    |
+--------------------------+-------+

SET GLOBAL innodb_lock_wait_timeout=1500;
```

https://blog.csdn.net/Maxiao1204/article/details/111022827

