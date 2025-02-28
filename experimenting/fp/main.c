#include <stdio.h>
typedef struct foo {
    char* data;
    void (*print)(struct foo *self);
} foo;

void foo_print(foo *self) {
    printf("%s\n", self->data);
}

int main(void) {
    foo obj = {"Hello, World!", foo_print};
    obj.print(&obj);
    return 0;
}
