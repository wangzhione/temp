BEGIN {
    item[101]="Github";
    item[22]="Whim";
    item[50]="Google";

    print "------Function: asort ------"
    total = asort(item, itemdesc);
    print "total:", total;
    for (i=1; i<=total; i++) {
        print "Index", i, "contains", itemdesc[i];
    }

    total = asorti(item, itemdesci);
    print "total:", total;
    print "------Function asorti------"
    for (i=1; i<=total; i++) {
        print "Index", i, "contains", itemdesci[i];
    }
}