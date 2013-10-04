--Lindsey Nguyen
--CSE 344 Homework 1
--Due October 3, 2013

--1.
--  a.
create table R(A int, B int);
--  b.
insert into R values(2, 4);
insert into R values(1, 1);
insert into R values(3, 2);
--  c.
select * from R;
--  d.
insert into R values('5', '2'); --No error because sqlite uses a dynamic type
-- system where the datatype is associated with the value itself not the container
--  e.
select A from R;
--  f.
select * from R where A <= B;

--2.
create table MyRestaurants(
Name varchar, 
TypeFood varchar,
Distance int,
Date varchar(10),
iLike int);

--3.
insert into MyRestaurants values('UT', 'teriyaki', 10, '2012-06-06', 0);
insert into MyRestaurants values('Ichiro', 'teriyaki', 11, '2012-04-06', 0);
insert into MyRestaurants values('Thai 66', 'thai', 15, '2013-04-08', 1);
insert into MyRestaurants values('Whateven', 'blah', 100, '2011-04-08', null);
insert into MyRestaurants values('Home', 'viet', 0, '2013-09-27', 1);

--4.
select * from MyRestaurants;

--5.
--  a.
.mode csv
select * from MyRestaurants;
--  b.
.separator |
.mode list
select * from MyRestaurants;
--c.
.width 15 15 15 15 15
.mode column
select * from MyRestaurants;
--  d.
.header on
select * from MyRestaurants;
.header off
select * from MyRestaurants;
--6.
select *, case when iLike = 1 then 'I liked it' else 'I hated it' end from MyRestaurants; 
--7.
select * from MyRestaurants where iLike = 1 and Date < date('now', '-3 month');
