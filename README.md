# MINI DBMS           

---

一个迷你型的简单DBMS数据库管理系统。                                                                                                                 by：ORACLE谢谢你

平台工具：VisualStudio 2019

项目内容：DBMS基本功能实现

## 功能实现：

1. 基本数据库操作
2. 基本表操作
3. 基本字段操作
4. 查询操作
5. 索引创建
6. 完整性约束
   1. 数据类型：支持三种基本数据类型：INT，CHAR(N)，FLOAT，其中CHAR(N)满足 1 <= N <= 255； 
   2. 一个表最多可以定义32个属性，各属性可以指定是否为UNIQUE；支持单属性的主键定义； 

## 后续完善：

## 附加内容：

## 说明：

1. 所有的命令均以英文符 ; 作为结尾

## 实例补充：

help;

create table student (
		sno char(8),
		sname char(16) unique,
		sage int,
		sgender char (1),
		primary key ( sno )
);

select * from student;

insert into student values ('11111111','a',10,'A');
insert into student values ('22222222','bb',20,'B');
insert into student values ('33333333','ccc',30,'C');
insert into student values ('44444444','dddd',40,'D');
insert into student values ('55555555','eeeee',50,'E');
insert into student values ('66666666','ffffff',60,'F');
insert into student values ('77777777','ggggggg',70,'G');
insert into student values ('88888888','hhhhhhhh',80,'H');
insert into student values ('99999999','iiiiiiiii',90,'I');

select * from student where sage > 20 and sgender = 'F';

delete from student where sno = '88888888';

select * from student where sno = '88888888';

create index stunameidx on student ( sname );

select sname from student where sage > 20 and sgender = 'F';

drop index stunameidx;

drop table student;

