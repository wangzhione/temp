BEGIN {
    item[101]="Github";
    item[21]="Google";
    for (x in item) {
        print item[x];
    }

    item["1,1"]="Github";
    item["1,2"]="Google";
    item["2,1"]="Whim";
    delete item["2,1"];
    for (x in item) {
        print "Index", x, "contains", item[x];
    }
}