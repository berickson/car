import qbs

CppApplication {
    consoleApplication: true
    files: "main.cpp"

    Group {     // Properties for the produced executable
        fileTagsFilter: product.type
        qbs.install: true
    }
}
