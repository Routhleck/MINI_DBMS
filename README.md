# 数据库课程设计
功能进度：
- [x] SQL语句解析
- [x] 数据库创建
- [x] 数据库删除
- [x] 表的创建
- [x] 表的删除
- [x] 字段添加
- [x] 字段删除
- [x] 字段修改
- [x] 记录插入
- [x] 记录插入（多重）
- [x] 记录删除
- [x] 记录删除（多重）
- [x] 记录修改
- [x] 选择全部
- [x] 选择指定列
- [x] 选择where条件
- [x] GUI
- [ ] 读取脚本
- [ ] 索引
- [ ] 视图
- [x] 用户权限
- [ ] 完整性约束

## 使用run.py文件运行
右下角输入指令，submit等于回车

点击数据库按钮刷新指令

点击全选选择所有数据库

点击加载加载数据

加载完成后可在data中查看每个数据库以及sheet
## 登录管理员
username:admin
username:admin
## 创建数据库
create database {database_name}

eg.: create database test_db
## 删除数据库
drop database {database_name}

eg.: drop database test_db
## 使用数据库
use database {database_name}

eg.: use database test_db
## 创建表
create table {table_name} ({column_name} {data_type} {PK,null...},{column_name} {data_type} {PK,null...}...)

eg.: create table test (v1 int PK null,v2 int)
## 删除表
drop table {table_name}

eg.: drop table test
## 添加字段
alter {table_name} add ({column_name} {data_type} {PK,null...})

eg.: alter test add (v3 int)
## 删除字段
alter {table_name} drop ({column_name})

eg.: alter test drop (v3)
## 修改字段
alter {table_name} modify {alter_field_name} ({column_name} {data_type} {PK,null...}) 

eg.: alter test modify v1 (v3 int PK null)
## 记录插入
insert into {table_name} {column_name=value,column_name=value,...)

eg.: insert into test v1=1,v2=2
## 记录插入（多重）
insert into {table_name} {column_name=value,column_name=value,...&column_name=value,column_name=value,...)

eg.: insert into test v3=2,v2=4&v3=3,v2=5
## 记录删除
delete on {table_name} where {column_name=value或column_name>value或column_name<value}

eg.: delete on test where v3=1
## 记录删除（多重）
delete on {table_name} where {column_name=value或column_name>value或column_name<value,column_name=value或column_name>value或column_name<value,..}

eg.: delete on test where v3=1&v2=2
## 记录修改
update {table_name} set column_name=value,column_name=value,... where {column_name=value或column_name>value或column_name<value（可多重）}

eg.: update test set v3=4,v2=3 where v3=2
## 选择全部
select * from {table_name}

eg.: select * from test
## 选择指定列
select {column_name} from {table_name}

eg.:select v3 from test
## 选择where条件
select * 或{column_name} from {table_name} where {column_name=value或column_name>value或column_name<value（可多重）}

eg.: select * from test where v3=4

## 注册用户
signup {username} {password}

eg.: signup admin admin
