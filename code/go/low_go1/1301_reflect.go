package main

import (
	"fmt"
	"reflect"
	"strconv"
)

// build:
// go run 1301_reflect.go
//
func main() {
	s := Student{
		Age:  20,
		Name: "jonson",
	}
	fmt.Println(s.CreateSQL())

	createQuery(Student{Age: 200, Name: "bun"})

	// syntax error: unexpected const, expecting type
	// var i = 0
	// var iptr const *int = &i
	// println(iptr)
}

// Student 练习学生结构体
type Student struct {
	Age  int
	Name string
}

/*
单引号
用来限定字符串。如果将值与字符串类型的列进行比较，则需要限定符号(也就是是单引号)。
用来与数值列进行比较的值不需要用引号.不管使用何种形式的字符串类型，
值都必须括在引号内（通常单引号更好）。

双引号
在 SQL 标准中是不存在双引号的，往往是数据库对SQL的扩展,
当前大部分数据库系统也是接受双引号的,作为字符串类型的限定符效果等同于单引号.
在 MySQL 数据中当单引号被用作限定符使用时,使用双引号和使用单引号效果是等价的.
*/

// CreateSQL insert sql
func (s *Student) CreateSQL() string {
	sqlStr := fmt.Sprintf("insert into student values(%d, '%s')", s.Age, s.Name)
	return sqlStr
}

// SQL interface
type SQL interface {
	CreateSQL() string
}

func createQuery(q interface{}) string {
	var value reflect.Value = reflect.ValueOf(q)
	// 判断类型为结构体
	if value.Kind() == reflect.Struct {
		// 获取结构体名字
		typeName := reflect.TypeOf(q).Name()
		// 插入语句
		query := fmt.Sprintf("insert into %s values(", typeName)

		if value.NumField() > 0 {
			field := value.Field(0)
			switch field.Kind() {
			case reflect.Int:
				query += strconv.FormatInt(field.Int(), 10)
			case reflect.String:
				query += "'" + field.String() + "'"
			}

			for i := 1; i < value.NumField(); i++ {
				field := value.Field(i)
				switch field.Kind() {
				case reflect.Int:
					query += ", " + strconv.FormatInt(field.Int(), 10)
				case reflect.String:
					query += ", '" + field.String() + "'"
				}
			}
		}

		query += ")"

		fmt.Println(query)

		return query
	}
	return ""
}
