# 2021q1 Homework1 (lab-0)
contributed by < `qwe661234` >
###### tags: `linux2021`
## 作業要求
q_insert_tail 和 q_size 需要將原本 O(n) 時間複雜度的實作改寫為 O(1)時間複雜度

## 測試
測試所有的測資:
> make test

針對單一個測試可以用:
> ./qtest -f <command file>

得到更詳細的程式運行狀況，方便修改程式的錯誤


## 實作
### queue structure

``` c=
typedef struct {
    list_ele_t *head; 
    list_ele_t *tail;
    int size;
} queue_t;
```

### q_new
配置記憶體產生一個新的 queue，並檢查記憶體是否成功配置
``` c=
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (q == NULL) {
        return NULL;
    }
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}
```
### q_free
釋放 queue 的記憶體，釋放前要先把 queue 裡面存的 data 佔用的記憶體先釋放掉
``` c=
void q_free(queue_t *q)
{
    if (q == NULL) {
        return;
    }
    list_ele_t *node;
    node = q->head;
    while (node) {
        list_ele_t *current = node;
        node = node->next;
        free(current->value);
        free(current);
    }
    free(q);
}
```
### q_insert_head
新增一個 list_ele 和配置記憶體給為其存取的字串，並將其設置為新的開頭，需檢查 q 是否存在以及是否有成功配置記憶體給新增的 list_ele 及其存取的字串 
``` c=
bool q_insert_head(queue_t *q, char *s)
{
    list_ele_t *newh;
    if (q == NULL) {
        return false;
    }
    newh = malloc(sizeof(list_ele_t));
    if (newh == NULL) {
        return false;
    }
    int slen = strlen(s);
    newh->value = malloc((slen + 1) * sizeof(char));
    if (newh->value == NULL) {
        free(newh);
        return false;
    }
    strncpy(newh->value, s, slen);
    newh->value[slen] = '\0';
    newh->next = q->head;
    q->head = newh;
    q->size++;
    if (q->size == 1) {
        q->tail = q->head;
    }
    return true;
}
```
### q_insert_tail
方法和 insert_head 相似，只是改成將其設為結尾，新增變數 q->tail 可使此 function 的時間複雜度達到 O(1)，檢查部分要多檢查 q_size 是否為 0，如果不為0則將開和結尾都設成此 list_ele
``` c=
bool q_insert_tail(queue_t *q, char *s)
{
    list_ele_t *newt;
    if (q == NULL) {
        return false;
    }
    newt = malloc(sizeof(list_ele_t));
    if (newt == NULL) {
        return false;
    }
    int slen = strlen(s);
    newt->value = malloc((slen + 1) * sizeof(char));
    if (newt->value == NULL) {
        free(newt->value);
        free(newt);
        return false;
    }
    strncpy(newt->value, s, slen);
    newt->value[slen] = '\0';
    newt->next = NULL;
    if (q->size == 0) {
        q->head = newt;
        q->tail = newt;
    } else {
        q->tail->next = newt;
        q->tail = newt;
    }
    q->size++;
    return true;
}
```
### q_remove_head 
將 head 移除，移除需要將原本存在 head 中的內容存於 sp，儲存時要先確認 buffer 大小以免造成記憶體錯誤
``` c=
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (q == NULL || q->head == NULL) {記憶體使用ㄅㄨ
        return false;
    }
    list_ele_t *target = q->head;
    if (sp != NULL) {
        int slen = strlen(target->value);
        if (bufsize >= (slen + 1)) {
            strncpy(sp, target->value, slen);
            sp[slen] = '\0';
        } else {
            strncpy(sp, target->value, bufsize - 1);
            sp[bufsize - 1] = '\0';
        }
    }
    q->head = q->head->next;
    q->size--;
    if (q->size == 0) {
        q->tail = NULL;
    }
    free(target->value);
    free(target);
    return true;
}
```
### q_size
用變數 q->size 來 maintain，可以達到 O(1)
``` c=
int q_size(queue_t *q)
{
    if (q == NULL) {
        return 0;
    }
    return q->size;
}
```
### q_reverse
front 用來存前面的 element, back 用來存後面的 element， 將 back 的 next 指向 front，但在指向之前要先存在 temp 中，否則 next 會指向 front 後無法繼續往下一個 element 做 reverse
``` c=
void q_reverse(queue_t *q)
{
    if (q == NULL || q->head == q->tail) {
        return;
    }
    list_ele_t *front, *back, *temp;
    front = q->head;
    back = front->next;
    q->tail = q->head;
    while (back != NULL) {
        temp = back->next;
        back->next = front;
        if (front == q->head) {
            front->next = NULL;
        }
        front = back;
        back = temp;
    }
    q->head = front;
}
```
### q_sort
以類似龜兔賽跑的方式切分 list，fast 會一次跑兩個 list_ele, 而 slow 只會一次跑一個，以此方式將 queue 切成兩份。以此方式遞迴切分至 queue 的 element 只剩一個或空的，接著將其連接並排序，連接完成後找到 queue 的尾巴並 assign 給 q_tail 
``` c=
list_ele_t *merge(list_ele_t *l1, list_ele_t *l2)
{
    list_ele_t *newh = l1;
    if (strcmp(l1->value, l2->value) <= 0) {
        l1 = l1->next;
    } else {
        newh = l2;
        l2 = l2->next;
    }
    list_ele_t *ptr = newh;
    while (1) {
        if (l1 != NULL && l2 != NULL) {
            if (strcmp(l1->value, l2->value) <= 0) {
                ptr->next = l1;
                l1 = l1->next;
            } else {
                ptr->next = l2;
                l2 = l2->next;
            }
        } else {
            if (l2 == NULL) {
                ptr->next = l1;
            } else {
                ptr->next = l2;
            }
            break;
        }
        ptr = ptr->next;
    }
    return newh;
}
list_ele_t *mergeSortList(list_ele_t *head)
{
    // merge sort
    if (head == NULL || head->next == NULL) {
        return head;
    }
    list_ele_t *fast = head->next;
    list_ele_t *slow = head;
    // split list
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;

    // sort each list
    list_ele_t *l1 = mergeSortList(head);
    list_ele_t *l2 = mergeSortList(fast);

    // merge sorted l1 and sorted l2
    return merge(l1, l2);
}
void q_sort(queue_t *q)
{
    if (q == NULL || q->head == q->tail) {
        return;
    }
    q->head = mergeSortList(q->head);
    list_ele_t *ptr = q->head;
    while (ptr->next != NULL) {
        ptr = ptr->next;
    }
    q->tail = ptr;
}
```
原本 function merge 是用 recursive 的方式，但由於效能不加，無法通過第15個測資，所以改以 nonrecursive 的方式改寫
```c=
list_ele_t *merge(list_ele_t *l1, list_ele_t *l2)
{
    // merge with recursive
    if (l2 == NULL) {
        return l1;
    }
    if (l1 == NULL) {
        return l2;
    }
    if (strcmp(l1->value, l2->value) <= 0) {
        l1->next = merge(l1->next, l2);
        return l1;
    } else {
        l2->next = merge(l1, l2->next);
        return l2;
    }
}
```
## 測試結果
``` c=
+++ TESTING trace trace-01-ops:
# Test of insert_head and remove_head
---	trace-01-ops	6/6
+++ TESTING trace trace-02-ops:
# Test of insert_head, insert_tail, and remove_head
---	trace-02-ops	6/6
+++ TESTING trace trace-03-ops:
# Test of insert_head, insert_tail, reverse, and remove_head
---	trace-03-ops	6/6
+++ TESTING trace trace-04-ops:
# Test of insert_head, insert_tail, size, and sort
---	trace-04-ops	6/6
+++ TESTING trace trace-05-ops:
# Test of insert_head, insert_tail, remove_head, reverse, size, and sort
---	trace-05-ops	5/5
+++ TESTING trace trace-06-string:
# Test of truncated strings
---	trace-06-string	6/6
+++ TESTING trace trace-07-robust:
# Test operations on NULL queue
---	trace-07-robust	6/6
+++ TESTING trace trace-08-robust:
# Test operations on empty queue
---	trace-08-robust	6/6
+++ TESTING trace trace-09-robust:
# Test remove_head with NULL argument
---	trace-09-robust	6/6
+++ TESTING trace trace-10-malloc:
# Test of malloc failure on new
---	trace-10-malloc	6/6
+++ TESTING trace trace-11-malloc:
# Test of malloc failure on insert_head
---	trace-11-malloc	6/6
+++ TESTING trace trace-12-malloc:
# Test of malloc failure on insert_tail
---	trace-12-malloc	6/6
+++ TESTING trace trace-13-perf:
# Test performance of insert_tail
---	trace-13-perf	6/6
+++ TESTING trace trace-14-perf:
# Test performance of size
---	trace-14-perf	6/6
+++ TESTING trace trace-15-perf:
# Test performance of insert_tail, size, reverse, and sort
---	trace-15-perf	6/6
+++ TESTING trace trace-16-perf:
# Test performance of sort with random and descending orders
# 10000: all correct sorting algorithms are expected pass
# 50000: sorting algorithms with O(n^2) time complexity are expected failed
# 100000: sorting algorithms with O(nlogn) time complexity are expected pass
---	trace-16-perf	6/6
+++ TESTING trace trace-17-complexity:
# Test if q_insert_tail and q_size is constant time complexity
Probably constant time
Probably constant time
---     trace-17-complexity     5/5
---	TOTAL		100/100

```

## Address Sanitizer

Error message:
``` c=
cmd> # Test if q_insert_tail and q_size is constant time complexity
cmd> option simulation 1
=================================================================
==19402==ERROR: AddressSanitizer: global-buffer-overflow on address 0x56148a3fe5e0 at pc 0x56148a3e6ae8 bp 0x7ffee2a2ab40 sp 0x7ffee2a2ab30
READ of size 4 at 0x56148a3fe5e0 thread T0
    #0 0x56148a3e6ae7 in do_option_cmd /home/qwe661234/linux2021/lab0-c/console.c:369
    #1 0x56148a3e58d0 in interpret_cmda /home/qwe661234/linux2021/lab0-c/console.c:221
    #2 0x56148a3e60b5 in interpret_cmd /home/qwe661234/linux2021/lab0-c/console.c:244
    #3 0x56148a3e72e1 in cmd_select /home/qwe661234/linux2021/lab0-c/console.c:594
    #4 0x56148a3e785b in run_console /home/qwe661234/linux2021/lab0-c/console.c:667
    #5 0x56148a3e43e5 in main /home/qwe661234/linux2021/lab0-c/qtest.c:780
    #6 0x7f13bcc7d0b2 in __libc_start_main (/lib/x86_64-linux-gnu/libc.so.6+0x270b2)
    #7 0x56148a3e1b8d in _start (/home/qwe661234/linux2021/lab0-c/qtest+0x8b8d)

0x56148a3fe5e1 is located 0 bytes to the right of global variable 'simulation' defined in 'console.c:21:6' (0x56148a3fe5e0) of size 1
  'simulation' is ascii string ''
SUMMARY: AddressSanitizer: global-buffer-overflow /home/qwe661234/linux2021/lab0-c/console.c:369 in do_option_cmd
Shadow bytes around the buggy address:
  0x0ac311477c60: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0ac311477c70: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0ac311477c80: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0ac311477c90: f9 f9 f9 f9 00 f9 f9 f9 f9 f9 f9 f9 00 f9 f9 f9
  0x0ac311477ca0: f9 f9 f9 f9 00 f9 f9 f9 f9 f9 f9 f9 00 f9 f9 f9
=>0x0ac311477cb0: f9 f9 f9 f9 00 f9 f9 f9 f9 f9 f9 f9[01]f9 f9 f9
  0x0ac311477cc0: f9 f9 f9 f9 00 00 00 00 01 f9 f9 f9 f9 f9 f9 f9
  0x0ac311477cd0: 04 f9 f9 f9 f9 f9 f9 f9 00 00 00 00 00 00 00 00
  0x0ac311477ce0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0ac311477cf0: 00 f9 f9 f9 f9 f9 f9 f9 01 f9 f9 f9 f9 f9 f9 f9
  0x0ac311477d00: 01 f9 f9 f9 f9 f9 f9 f9 04 f9 f9 f9 f9 f9 f9 f9
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
  Shadow gap:              cc
==19402==ABORTING

```
從 terminal有看出問題出在 console.c:369
``` c=
int oldval = *plist->valp;
```
但一直找不到原因，後來參考其他人的開發紀錄才知道原因出在上面宣告的 simulation 是 1個 bytes 的 bool， 而下面是用 int 來讀，並以４個 bytes 的方式存取，所以會出現此錯誤，改動三處型別即可解決
``` c=
bool simulation = false; -> int simulation = false;
extern bool simulation; -> extern int simulation;
static bool echo = 0; -> static int echo = 0;
```
## Valgrind & Massif

執行Valgrind
>make valgrind
>valgrind --tool=massif ./qtest -f testfile
>massif-visualizer outputfile

測資16的 outputfile
![](https://i.imgur.com/RLbudWU.png)

測資17的 outputfile
![](https://i.imgur.com/7qyV9YG.png)
![](https://i.imgur.com/0AV3Kks.png)


## CERN 筆記
許多 vulnerability 關於buffer overflow 和 string mauniplation. 這大多會以segmentation fault 來呈現 Error.

gets, strcpy, sprintf 不安全原因為未檢查 buffer長度，且 strcpy 可能 overwrite 目標字串相鄰的記憶體位置。
1. gets -> fgets
2. strcpy -> strlcpy 
或是 strcpy -> strncpy 
但第二個方法較麻煩，因為他不會自動產生結尾字元 '\0'，要自己另外檢查並接上。
3. sprintf() -> snprintf()
strcat 和 strcmp 也有風險存在，但未提及修改方式
4. printf format string Ex: print(argv[1]) 
```
#FormatString.c
#include <stdio.h>
 
int main(int argc, char **argv) {
    char *secret = "This is a secret!\n";
 
    printf external link(argv[1]);
 
    return 0;
}
```
當輸入 ./FormatString %s 出現意想不到的結果
```
$ gcc -mpreferred-stack-boundary=2 FormatString.c -o FormatString
$ ./FormatString %s
This is a secret!
```
所以最好將 format string 寫死，不要讓 user 自己去 input
5. file openiing
Symbolic link attack ([symbolic link 是什麼](https://en.wikipedia.org/wiki/Symbolic_link))
```
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
 
#define MY_TMP_FILE "/tmp/file.tmp"
 
 
int main(int argc, char* argv[])
{
    FILE * f;
    if (!access(MY_TMP_FILE, F_OK)) {
        printf external link("File exists!\n");
        return EXIT_FAILURE;
    }
    /* At this point the attacker creates a symlink from /tmp/file.tmp to /etc/passwd */
    tmpFile = fopen(MY_TMP_FILE, "w");
 
    if (tmpFile == NULL) {
        return EXIT_FAILURE;
    }
 
    fputs("Some text...\n", tmpFile);
 
    fclose(tmpFile);
    /* You successfully overwrote /etc/passwd (at least if you ran this as root) */
 
    return EXIT_SUCCESS;
}
```
這樣你的密碼就被偷走了，所以 open file 前要先用 unlink() 將可能存在的 symbolic link 去除，並且不要 overwrite 已存在的檔案

reference: https://security.web.cern.ch/recommendations/en/codetools/c.shtml


    
