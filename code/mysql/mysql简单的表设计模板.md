# MySQL CREATE TABLE 简单设计模板交流

推荐用 MySQL 8.0 (2018/4/19 发布, 开发者说同比 5.7 快 2 倍) 或同类型以上版本.

## CREATE TABLE TEMPLATE

```SQL
CREATE TABLE [table_name] (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY COMMENT '物理主键',
    update_time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
    create_time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    [delete_time TIMESTAMP DEFAULT NULL COMMENT '删除时间',]

    [template]

) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT = '模板表';
```

这个模板基本可以应对大部分业务场景. 后面我们也会分析小部分复杂场景. 

首先来分析下这个模板潜在考量.

**分析 1: 物理主键 id 为什么是 bigint unsigned ?**

交流 : 

- 1' 性能更好, unsigned 不涉及 反码和补码 转码消耗
- 2' 表示物理主键更广 [-2^63, 2^63-1] -> [0, 2^64-1]
- 3' mysql 优化会更好. select * from * where id < 250; 
     原先是 select * from * where -2^63 <= id and id < 250;
     现在是 select * from * where 0 <= id and id < 250;
- 4' 如果有些语言中没有 unsigned, 需要把关人备注为 signed 使用范围是 [0, 2^63-1]
- 5' 如果 int 也能满足业务, 也可以用 int, 节省 4 字节. 看业务把控和取舍.

**分析 2: COMMENT 是否可有可无 ?**

交流 : 

- 1' **对于缺少 COMMENT 详细注释的, 推荐把关人 或 DBA 打回修改或拒绝操作**
- 2' 修改和补充 COMMENT

```SQL
-- 修改表注释
ALTER TABLE [table_name] COMMENT '[COMMENT]';

-- 修改字段注释
UPDATE information_schema.COLUMNS SET column_comment = '[COMMENT]' 
    WHERE TABLE_SCHEMA= '[database_name]' 
        AND TABLE_NAME='[table_name]' AND COLUMN_NAME= '[column_name]'
```

**分析 3: 为什么用 timestamp 表示时间 ?**

交流 :

- 1' timestamp 和 int 一样都是 4 字节. 用它表示时间戳更精简更友好.
- 2' 业务不再关心时间的创建和更新相关业务代码. 省心, 省代码

**分析 4: 为什么是 utf8mb4 而不是 utf8 ?**

交流 : 

- 1' MySQL 的 utf8 不是标准的 utf8 unicode 字符集编码. 
     正规 UTF-8 编码是使用 1-6 字节表示 unicode 字符. 
     但 MySQL utf8 只使用了 1-3 字节表示一个字符, 
     那么当他遇到 4 字节编码以上的 unicode 字符, 如表情符号会发生意外. 
     所以 MySQL 在 5.5 之后版本推出了 utf8mb4 编码, 完全兼容以前的 utf8 编码. 

## CREATE TABLE 小部分场景

在我们筹备大型软件和服务设计时候, 需要有人对锁使用**特性与设计以及业务把关和负责**. 

这些小部分场景主要围绕在 AUTO_INCREMENT 设计取舍. 先科普下其相关特性.

### AUTO_INCREMENT

AUTO_INCREMENT id 我们习惯叫他 **自增 id**, 这种说法不准确容易引发误解. 
稍微具体点叫法**insert 频率 id**. 哪怕 insert 失败了, 这个 id 频率也会 +1 (or +auto_increment_increment).

> 倒排索引
> 同样这个通用名词也很人费解. 如果叫 **反向索引** 很好理解.
> 例如我们**普通的 id -> data 是(正向)索引, data 中关键 key -> id 是反向(Inverted index)索引**
> [倒排索引为什么叫倒排索引?](https://www.zhihu.com/question/23202010)

更加具体点, 我们看看 MySQL InnoDB 引擎中怎么定义和实现 AUTO_INCREMENT

#### **1. InnoDB 中 AUTO_INCREMENT 配置大致说明**

InnoDB 引擎中 有三种 AUTO_INCREMENT 锁模式

- 1.1 innodb_auto_inc_lock_mode=0 (traditional lock mode) 
  获取表锁 (AUTO-INC 锁, 特殊表锁), 语句执行结束后释放, 不需要等事务结束. 
  分配的值也是一个个分配，是连续的. 
  (如果事务 rollback 了这个 auto_increment 值就会浪费掉, 从而造成间隙)

- 1.2 innodb_autoinc_lock_mode=1 (consecutive lock mode, MySQL 8.0 之前默认选项)
  对于不确定插入数量的语句(例如 INSERT ... SELECT, REPLACE ... SELECT 和 LOAD DATA)和 innodb_autoinc_lock_mode=0 一样获取表锁, 
  其他的确定数量的语句在执行前先批量获取 id, 之后再走的是轻量级互斥锁, 
  如果其他事务已经获取表锁, 这个时候也需要等待.

- 1.3 innodb_autoinc_lock_mode=2 (interleaved lock mode, MySQL 8.0+ 默认)
  采用乐观锁, CAS 更新计数器获取. 
  正常情况性能最好, 因为没有表锁和轻量级互斥锁. 但在高请求, 高 CPU load 时候会适得其反. 
  AUTO_INCREMENT cas 频率高, 同一个语句操作内部 id cas inc 间隙变大概率很高.

#### **2. InnoDB 中 AUTO_INCREMENT 实现大致思路**

在 MySQL 8.0 之前 AUTO_INCREMENT 值存储在内存中. 每次**重启**通过 select max id 初始化值.

```SQL
-- 大致方式通过行级锁(排他锁) MAX(id) -> AUTO_INCREMENT init value
SELECT MAX(ai_col) FROM [table_name] FOR UPDATE;
```

在 MySQL 8.0 之后, 持久化存储在磁盘. 每次更新会写入 redo log 中, 也会刷入 innodb 引擎系统表中记录下来. 

如果 MySQL 正常关闭重启, 会从引擎系统表中获取计数器的值.

如果 MySQL 故障重启, 也会从引擎系统表中获取计数器的值; 
并且从最后一个检查点开始扫描 redo log 中记录的计数器值; 取二者最大值作为新值. 

但是这个处理逻辑也不能保证最后拿到的值是正确的. 如果在系统文件落盘前崩溃, 
那么就可能拿到一个之前使用过的值. 这也是数据备份和同步时候可能引发主键冲突根源.

### 小部分场景

通过对 InnoDB 的 AUTO_INCREMENT 了解, 大致猜测到他的优缺点和使用领域以及现状. 

#### **交流 1: 如果考虑分布式场景呢, 高性能领域呢 ?**

这时候推荐使用分布式唯一 ID 生成算法器. (用更复杂大炮干复杂长枪) 替代 AUTO_INCREMENT.

补充说明, 在普通领域 AUTO_INCREMENT 也是个长枪级别 ID 生成器.

**原理**

```SQL
-- sequece id 生成器表
CREATE TABLE sequece (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY COMMENT '物理主键, 自增 id',
    stub char(1) NOT NULL DEFAULT '' COMMENT '打桩靶子',
    UNIQUE KEY unique_key_stub (stub)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT = '古老分布式 id 生成器';

-- 获取 id
DELIMITER $
BEGIN
REPLACE INTO sequece(stub) VALUES ('X');
SELECT LAST_INSERT_ID();
COMMIT
$
```

**部署**

我们也可以多台机器部署, 设置不同 AUTO_INCREMENT step, 让每个 sequece 产生不同号码.
例如部署 step = 2 个服务结点, 并行获取数据. 
一个 from 1, 3, 5, 7, 9 ... 
一个 from 2, 4, 6, 8, .. .

详细部署操作指导

```SQL
-- step 标识增长步长, 也标识分布式机器数

show global variables like 'auto_increment%'

+--------------------------+-------+
| Variable_name            | Value |
+--------------------------+-------+
| auto_increment_increment | 1     |
| auto_increment_offset    | 1     |
+--------------------------+-------+

-- auto_increment_increment 全局步长
-- auto_increment_offset 自增起始值

-- 设定自增步长
-- set session 设置当前会话链接, set global 设置当前 ID 机器
set global auto_increment_increment=step

for i : [0, step)
    CREATE TABLE sequece (
        id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY COMMENT '物理主键, 自增 id',
        stub char(1) NOT NULL DEFAULT '' COMMENT '打桩靶子',
        UNIQUE KEY unique_key_stub (stub)
    ) ENGINE=InnoDB AUTO_INCREMENT = [offset + i] DEFAULT CHARSET=utf8mb4 COMMENT = '古老分布式 id 生成器';
```

这种古老 MySQL 分布式 ID 生成器, 方案成熟部署简单. 存在 DB 性能瓶颈. 如果考虑高可用主从架构,
在主服务挂了, **从服务顶上时候存在重复发号可能**.  

关于 分布式唯一 ID 其它业界解决方案以及我们中国区解决方案未来解决方案, 后面有机会再聊.

#### **交流 2: 为什么官方推荐 AUTO_INCREMENT 当主键, 而很少见到 UUID 等等?**

MySQL InnoDB 引擎默认主键索引是 B+ 线索树索引, 也称为聚簇索引(聚集索引, row key 和 row value pointer 存在连续内存中), 为何叫聚簇索引呢?

在 InnoDB 中, 每个表都会有一个聚簇索引, 在定义了主键(primary key)的情况下, 主键所在的列会被作为聚簇索引存储. 所谓聚簇索引，意思是数据实际上是存储在索引的叶子节点上,「聚簇」的含义就是和相邻的数据紧凑地存储在一起. 因为不值得同时把数据行存储在两个不同的位置，所以一个表只能有一个聚簇索引.

关于 InnoDB 选择哪个列作为聚簇索引存储，大概的优先级为:
如果定义了主键(primary key), 则使用主键;
如果没有定义主键, 则选择第一个不包含 NULL(NOT NULL)的 UNIQUE KEY;
如果也没有, 则会隐式定义一个主键作为聚簇索引.

[聚簇索引说明](https://xlzd.me/2018/09/14/why-mysql-innodb-need-auto-increment-primary-key/)

MySQL 读取磁盘上的数据是一页一页读取的, 如果某条我们要处理的数据在某一页中, 
但是这一页其他数据我们都不关心, 这样的请求多了, 性能会急剧下降, 类似于 CPU 的并发杀手 false sharing.

> 伪共享 (false sharing) 的非标准定义为: 
> 缓存系统中是以缓存行 (cache line) 为单位存储的. 当多线程修改互相独立的变量时, 
> 如果这些变量共享同一个缓存行, 就会无意中影响彼此的性能, 这就是伪共享.

按照 B+ 线索平衡树的原理, AUTO_INCREMENT 的 ID 能保证最新的数据在一页中被读取, 而且减少了 B+ 树分裂翻转. UUID 等唯一 ID 由于无序, 插入时, B+ 树会不断翻转, 并且最新的数据可能不在同一页. 很可能会出现, 最新一条数据, 和好几年前的数据在同一页.

例如购物和支付交易的订单, 节日促销的抽奖活动这类业务都有这样的使用场景, 访问频率在最近一天, 一周, 或者几个月内比较活跃, 而超过一段时间内的数据很少访问. 当然架构设计是当下业务和未来业务场景之间取舍. 抛开 MySQL AUTO_INCREMENT 的 ID 分布式和锁性能瑕疵, 在尝试分库分表时候他就变得有点累赘. (TiDB 中另说, 解决了一些问题, 也会引入新的问题)

## 后记

公司业务复杂度也在逐年上升, 行业技术升级变革同样也是是有条不絮. 
这里只是简单以 CREATE TABLE 为切入点同大家交流, 欢迎补充纠错, 提升公司技术稳定性, 便利性, 降低业务拓展中技术债负担. 
