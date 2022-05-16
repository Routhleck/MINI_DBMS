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
## 登录管理员
username:admin
username:admin
## 创建数据库
create database {database_name}
## 删除数据库
drop database {database_name}
## 使用数据库
use database {database_name}
## 创建表
create table {table_name} ({column_name} {data_type} {PK,null...},{column_name} {data_type} {PK,null...}...)
## 删除表
drop table {table_name}
## 添加字段
alter {table_name} add ({column_name} {data_type} {PK,null...})
## 删除字段
alter {table_name} drop ({column_name})
## 修改字段
alter {table_name} modify {alter_field_name} ({column_name} {data_type} {PK,null...}) 
## 记录插入
insert into {table_name} {column_name=value,column_name=value,...)
## 记录插入（多重）
insert into {table_name} {column_name=value,column_name=value,...&column_name=value,column_name=value,...)
## 记录删除
delete on {table_name} where {column_name=value或column_name>value或column_name<value}
## 记录删除（多重）
delete on {table_name} where {column_name=value或column_name>value或column_name<value&column_name=value或column_name>value或column_name<value&..}
## 记录修改
update {table_name} set (column_name=value,column_name=value,...) where {column_name=value或column_name>value或column_name<value（可多重）}
## 选择全部
select * from {table_name}
## 选择指定列
select {column_name} from {table_name}
## 选择where条件
select * 或{column_name} from {table_name} where {column_name=value或column_name>value或column_name<value（可多重）}
