package main

// build:
// go run 13_variable.go
//

var d uint8

func main() {
	var a uint8 = 1
	a = 2
	if true {
		a = 3
	}
	d = a
}

// GOSSAFUNC=main GOOS=linux GOARCH=amd64 go tool compile 13_variable.go
//

// go tool compile -S 13_variable.go
//

// go help buildmode
/*
The 'go build' and 'go install' commands take a -buildmode argument which
indicates which kind of object file is to be built. Currently supported values
are:

        -buildmode=archive
                Build the listed non-main packages into .a files. Packages named
                main are ignored.

        -buildmode=c-archive
                Build the listed main package, plus all packages it imports,
                into a C archive file. The only callable symbols will be those
                functions exported using a cgo //export comment. Requires
                exactly one main package to be listed.

        -buildmode=c-shared
                Build the listed main package, plus all packages it imports,
                into a C shared library. The only callable symbols will
                be those functions exported using a cgo //export comment.
                Requires exactly one main package to be listed.

        -buildmode=default
                Listed main packages are built into executables and listed
                non-main packages are built into .a files (the default
                behavior).

        -buildmode=shared
                Combine all the listed non-main packages into a single shared
                library that will be used when building with the -linkshared
                option. Packages named main are ignored.

        -buildmode=exe
                Build the listed main packages and everything they import into
                executables. Packages not named main are ignored.

        -buildmode=pie
                Build the listed main packages and everything they import into
                position independent executables (PIE). Packages not named
                main are ignored.

        -buildmode=plugin
                Build the listed main packages, plus all packages that they
                import, into a Go plugin. Packages not named main are ignored.

On AIX, when linking a C program that uses a Go archive built with
-buildmode=c-archive, you must pass -Wl,-bnoobjreorder to the C compiler.
*/

/*
 -buildmode=archive : 将非 main package 构建为 .a 文件, .main 包将被忽略

 -buildmode=c-archie : 将 main package 及其导入的所有 package 都构建到 C 归档文件中

 -buildmode=c-shared : 将 main package 以及他们导入的所有 package 都构建到 C 动态库中

 -buildmode=shard : 将所有非 main package 都合并到一个动态库中, 在使用 -linkshard 参数后, 能够使用此动态库

 -buildmode=exe : 将 main package 和其导入的 package 构建为可执行文件
*/

// go build -x 13_variable.go
