#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

int k=0;
typedef enum {RED, BLACK} Color;
typedef enum {LEFT, RIGHT} Rotation;
typedef struct RBNode {
    Color color;
    struct RBNode *left;
    struct RBNode *right;
    struct RBNode *parent;
    char word[];
} RBNode;

#define NIL &nil
RBNode nil =  {BLACK, NIL, NIL, NULL, ""};
RBNode *root = NIL;

struct Symbol {
    char s;
    int lengthOK, lengthKO;
    int num;
    bool exact;
    struct Symbol *next;
    int *posOK, *posKO;
};
struct Symbol *h = NULL;

void inorder(RBNode *r){
    if(r!=NIL){
        inorder(r->left);
        fprintf(stdout, "%s\n", r->word);
        inorder(r->right);
    } else
        return;
}
void rotateTree(RBNode *x, Rotation sense){
    RBNode *y, **z, **w;
    if(sense == LEFT){
        y = x->right;
        x->right = y->left;
        if(y->left != NIL) y->left->parent = x;
        z = &(x->parent->left);
        w = &(x->parent->right);
    }
    else {
        y = x->left;
        x->left = y->right;
        if(y->right != NIL) y->right->parent = x;
        z = &(x->parent->right);
        w = &(x->parent->left);
    }
    if(y != NIL)
        y->parent = x->parent;
    if(x->parent){
        if(x == *z)
            *z = y;
        else
            *w = y;
    }
    else
        root = y;
    if(sense == LEFT)
        y->left = x;
    else
        y->right=x;
    if(x !=NIL)
        x->parent = y;
}
void insertFixup(RBNode *x){
    while(x != root && x->parent->color == RED){
        if(x->parent == x->parent->parent->left){
            RBNode *y = x->parent->parent->right;
            if(y->color == RED){
                x->parent->color = BLACK;
                y->color = BLACK;
                x->parent->parent->color = RED;
                x = x->parent->parent;
            } else {
                if(x == x->parent->right){
                    x = x->parent;
                    rotateTree(x, LEFT);
                }
                x->parent->color = BLACK;
                x->parent->parent->color = RED;
                rotateTree(x->parent->parent, RIGHT);
            }
        } else{
            RBNode *y = x->parent->parent->left;
            if(y->color == RED){
                x->parent->color = BLACK;
                y->color = BLACK;
                x->parent->parent->color = RED;
                x = x->parent->parent;
            }else{
                if(x == x->parent->left){
                    x= x->parent;
                    rotateTree(x, RIGHT);}
                x->parent->color = BLACK;
                x->parent->parent->color = RED;
                rotateTree(x->parent->parent, LEFT);
            }
        }
    }
    root->color = BLACK;
}
bool compareWords (char *a, char *b){
    int count=0;
    while(*a==*b && count<k){
        a++;
        b++;
        count++;
    } return (*a<*b);
}
RBNode *insertRBNode(char *w){
    RBNode *cur, *parent;
    cur = root;
    parent = NULL;
    RBNode *x;
    x = malloc(sizeof(*x) + (k+1)*sizeof(char));
    for(int i = 0; i<k; i++)
        x->word[i]=w[i];
    x->word[k]= '\0';
    x->left = NIL;
    x->right = NIL;
    x->color = RED;
    while(cur !=NIL){
        parent = cur;
        if(compareWords(w, cur->word) == true)
            cur = cur->left;
        else
            cur = cur->right;
    }
    x->parent = parent;
    if(parent != NULL){
        if(compareWords(w, parent->word))
            parent->left = x;
        else
            parent->right = x;
    } else
        root=x;
    insertFixup(x);
    return(x);
}
bool belongsToGlossary(char *word){
    if(root == NIL)
        return 0;
    else{
        RBNode *t=root;
        do{
            char *c = t->word;
            char *w = word;
            while(*w == *c){
                if(*w=='\0' && *c == '\0')
                    break;
                w++;
                c++;
            }
            if(*w < *c)
                t=t->left;
            else{
                if(*w > *c)
                    t=t->right;
                else
                    return 1;
            }
        }while(t!=NIL);
        return 0;
    }
}

struct Parola{
    RBNode *address;
    struct Parola *next;
};
int N=0;
struct Parola *head = NULL; //lista parole filtrate
struct Parola *inserisci = NULL; //lista nuovi inserimenti

void insertAtBeginning(struct Parola **href, RBNode *new){
    struct Parola *p = (struct Parola*)malloc(sizeof(struct Parola));
    if(href == NULL)
        p->next = NULL;
    else
        p->next = (*href);
    p->address = new;
    (*href) = p;
}
void insertAfter(struct Parola *prev, RBNode *new){
    if(prev !=NULL){
        struct Parola *tmp = (struct Parola*)malloc(sizeof(struct Parola));
        tmp->address = new;
        tmp->next = prev->next;
        prev->next = tmp;
    }
}

void deleteWord(struct Parola *cur){
    struct Parola *tmp = cur->next;
    cur->next=(cur->next)->next;
    N=N-1;
    free(tmp);
}
void printList(struct Parola *w){
    while(w !=NULL){
        RBNode *tmp = w->address;
        fprintf(stdout, "%s\n", tmp->word);
        w=w->next;
    }
}

int checkPoseInArray(const int *a, int n, int pose){
    for(int i =0; i<n; i++){
        if(a[i]==pose)
            return 1;
    }
    return 0;
}

void verifySecondConstraint(char c, int pose, struct Parola **w){
    struct Parola *tmp;
    while(*w!=NULL && ((*w)->address)->word[pose]!=c){
        tmp = *w;
        *w = (*w)->next;
        N = N-1;
        free(tmp);
    }
    tmp = *w;
    if(tmp !=NULL)
        while(tmp->next !=NULL){
            if(((tmp->next)->address)->word[pose]!=c)
                deleteWord(tmp);
            else
                tmp = tmp->next;
        }
}
void insertSecondConstraint(struct Symbol *s, int pos, struct Parola **lista){
    if(s->posOK == NULL){
        s->posOK = (int*)malloc(sizeof(int));
        s->posOK[0]=pos;
        s->lengthOK = 1;
        verifySecondConstraint(s->s, pos, lista);
    }
    else{
        if(checkPoseInArray(s->posOK, s->lengthOK, pos)==0){
            s->lengthOK = s->lengthOK +1;
            s->posOK = (int*)realloc(s->posOK, s->lengthOK*sizeof(int));
            s->posOK[s->lengthOK-1] = pos;
            verifySecondConstraint(s->s, pos, lista);
        }
    }
}

void verifyThirdConstraint(char c, int pose, struct Parola **w){
    struct Parola *tmp;
    while(*w !=NULL && ((*w)->address)->word[pose]==c){
        tmp = *w;
        *w = (*w)->next;
        N=N-1;
        free(tmp);
    }
    tmp = *w;
    if(tmp != NULL)
        while(tmp->next != NULL){
            if(((tmp->next)->address)->word[pose]==c)
                deleteWord(tmp);
            else
                tmp = tmp->next;
        }
}
void insertThirdConstraint(struct Symbol *s, int pos, struct Parola **lista){
    if(s->posKO == NULL){
        s->posKO = (int*)malloc(sizeof(int));
        s->posKO[0] = pos;
        s->lengthKO = 1;
        verifyThirdConstraint(s->s, pos, lista);
    }
    else{
        if(checkPoseInArray(s->posKO, s->lengthKO, pos)==0){
            s->lengthKO = s->lengthKO +1;
            s->posKO = realloc(s->posKO, s->lengthKO*sizeof(int));
            s->posKO[s->lengthKO-1] = pos;
            verifyThirdConstraint(s->s, pos, lista);
        }
    }
}

int countOccurrences(RBNode *n, char s){
    int num = 0;
    char *x = n->word;
    while(x != NULL && *x != '\0'){
        x = strchr(x, (int) s);
        if(x){
            x++;
            num = num +1;
        }
    }
    return num;
}

void verifyFourthConstraint(struct Symbol *s, int n, struct Parola **w){
    struct Parola *tmp;
    while(*w !=NULL && countOccurrences((*w)->address, s->s) < n){
        tmp = *w;
        *w = (*w)->next;
        N = N - 1;
        free(tmp);
    }
    tmp = *w;
    if(tmp !=NULL)
        while(tmp->next !=NULL){
            if(countOccurrences((tmp->next)->address, s->s) < n)
                deleteWord(tmp);
            else
                tmp = tmp->next;
        }
}
void insertFourthConstraint(struct Symbol *s, int c, struct Parola **w){
    if(s->num < c){
        s->num = c;
        verifyFourthConstraint(s, c, w);
    }
    if(s->num < s->lengthOK){
        s->num = s->lengthOK;
        verifyFourthConstraint(s, s->num, w);
    }
}

void verifyFifthConstraint(struct Symbol *s, int n, struct Parola **w){
    struct Parola *tmp;
    while(*w != NULL && countOccurrences((*w)->address, s->s)!=n){
        tmp = *w;
        *w = (*w)->next;
        N = N-1;
        free(tmp);
    }
    tmp = *w;
    if(tmp !=NULL)
        while(tmp->next != NULL){
            if(countOccurrences((tmp->next)->address, s->s)!=n)
                deleteWord(tmp);
            else
                tmp = tmp->next;
        }
}
void insertFifthConstraint(struct Symbol *s, int c, struct Parola **w){
    if(s->num != c)
        s->num = c;
    s->exact = true;
    verifyFifthConstraint(s, c, w);
}

void verifyFirstConstraint(char s, struct Parola **w){
    struct Parola *tmp;
    while(*w != NULL && strchr(((*w)->address)->word, s) != NULL){
        tmp = *w;
        *w = (*w)->next;
        N = N-1;
        free(tmp);
    }
    tmp = *w;
    if(tmp != NULL)
        while(tmp->next != NULL){
            if(strchr(((tmp->next)->address)->word, s) != NULL)
                deleteWord(tmp);
            else
                tmp = tmp->next;
        }
}
char *insertFirstConstraint(char s, char *esclusi, struct Parola **lista){
    verifyFirstConstraint(s, lista);
    if(esclusi == NULL){
        char *tmp= (char*)malloc(2*sizeof(char));
        tmp[0]=s;
        tmp[1]='\0';
        esclusi = tmp;
        return esclusi;
    }
    else{
        int i = (int) strlen(esclusi);
        char *tmp=(char*)malloc((strlen(esclusi)+2)*sizeof(char));
        strncpy(tmp, esclusi, i);
        tmp[i]=s;
        tmp[i+1]='\0';
        free(esclusi);
        esclusi = tmp;
        return esclusi;
    }
}

struct Symbol *findSymbol(struct Symbol **href, char c){
    struct Symbol *cur = *href;
    if(cur != NULL){
        do{
            if(cur->s > c)
                return NULL;
            else{
                if(cur->s == c)
                    return cur;
                else
                    cur = cur->next;
            }
        }while(cur !=NULL);
    }
    return NULL;
}
struct Symbol *sortedInsert(char c, struct Symbol **sList){
    struct Symbol *new = (struct Symbol*)malloc(sizeof(struct Symbol));
    new->s=c;
    new->posOK=NULL;
    new->posKO=NULL;
    new->lengthOK=0;
    new->lengthKO=0;
    new->exact=false;
    new->num=0;
    if(*sList ==NULL || (*sList)->s > new->s){
        new->next=*sList;
        *sList=new;
    }
    else{
        struct Symbol *cur=*sList;
        while((cur->next !=NULL) && (new->s > cur->next->s)){
            cur =cur->next;
        }
        new->next =cur->next;
        cur->next =new;
    }
    return new;
}
void deleteSymbol(struct Symbol **href, char c){
    struct Symbol *temp = *href;
    if(temp != NULL && temp->s == c){
        *href =temp->next;
        free(temp->posOK);
        free(temp->posKO);
        free(temp);
    }else{
        struct Symbol *prev=NULL;
        while(temp != NULL && temp->s !=c){
            prev = temp;
            temp = temp->next;
        }
        if(temp !=NULL){
            prev->next = temp->next;
            free(temp->posOK);
            free(temp->posKO);
            free(temp);
        }
    }
}
struct Symbol *modifySymbol(char c){
    struct Symbol *s = findSymbol(&h, c);
    if(s !=NULL)
        return s;
    else
        return sortedInsert(c, &h);
}

int checkBelongsToSet(char *r, char c){
    if((c!='+') && (c!='|') && (c!='/')){
        char *x = strchr(r, (int) c);
        if(x == NULL)
            return 0;
        else
            return 1;
    }
    return -1;
}
void doesNotBelongToWord(char *p, int i){
    char s = p[i];
    p[i] = '/';
    char *z=strchr(p, (int)s);
    while(z!= NULL){
        if(p[z-p] == s){
            p[z-p] = '/';
        }
        z = strchr(p, (int)s);
    }
}
void belongsToWord(char *r, char *p, int i, struct Parola *lista){
    struct Symbol *tmp = modifySymbol(p[i]);
    int c=0, n=0;
    char s = p[i];
    char *x = strchr(r, (int)s);
    do{
        n +=1;
        if(p[x-r]==s){
            c +=1;
            p[x-r]='+';
            insertSecondConstraint(tmp, (int)(x-r), &lista);
        }
        r[x-r]='+';
        x=strchr(r, (int)s);
    }while(x!=NULL);
    char *y = strchr(p, (int) s);
    do{
        if(y != NULL){
            if(c != n){
                p[y-p] = '|';
                c +=1;
            }else{
                p[y-p] = '/';
                if(tmp->exact==false)
                    insertFifthConstraint(tmp, c, &lista);
            }
            insertThirdConstraint(tmp, (int)(y-p), &lista);
            y = strchr(p, (int) s);
        }
    }while(y!=NULL);
    insertFourthConstraint(tmp, c, &lista);
    head = lista;
}

bool checkPositions(struct Symbol *s, int flag, char c, const char *newWord){
    int l, *tmp;
    if(flag ==1){
        l = s->lengthOK;
        tmp = s->posOK;
        for(int i =0; i <l; i++){
            if(newWord[*tmp]!=c)
                return 0;
            else
                tmp++;
        }
        return 1;
    }else{
        l=s->lengthKO;
        tmp = s->posKO;
        for(int i = 0; i<l; i++){
            if(newWord[*tmp]==c)
                return 0;
            else
                tmp++;
        }
        return 1;
    }
}
bool checkConstraints(char *newWord, char *esclusi, RBNode *node){
    if(esclusi !=NULL){
        if(strpbrk(esclusi, newWord)!=NULL)
            return 0;
    }
    struct Symbol *s = h;
    while(s!=NULL){
        if(s->lengthOK>0 && checkPositions(s, 1, s->s, newWord)==0)
            return 0;
        if(s->lengthKO>0 && checkPositions(s, 0, s->s, newWord)==0)
            return 0;
        int num = countOccurrences(node, s->s);
        if(num < s->num && s->exact==false)
            return 0;
        if(num != s->num && s->exact==true)
            return 0;
        s=s->next;
    }
    return 1;
}

void createListFromTree(char *esclusi, RBNode *r, struct Parola **lista){
    if(r==NIL)
        return;
    createListFromTree(esclusi, r->right, lista);
    if(checkConstraints(r->word, esclusi, r)==1){
        insertAtBeginning(lista, r);
        N = N+1;
    }
    createListFromTree(esclusi, r->left, lista);
}

void insertInSubList(struct Parola **iList, RBNode *elem){
    struct Parola **pre = iList;
    struct Parola *cur, *p=NULL;
    if(*pre!=NULL){
        cur = *pre;
        while(cur!=NULL && compareWords(cur->address->word, elem->word)==true){
            p=cur;
            pre=&cur;
            cur=cur->next;
        }
        if(pre==iList)
            insertAtBeginning(iList, elem);
        else
            insertAfter(p, elem);
    } else
        insertAtBeginning(iList, elem);
    N = N+1;
}
void mergeLists(struct Parola **lista, struct Parola **iList){
    if(*iList==NULL)
        return;
    struct Parola *nextI=(*iList)->next;
    if(compareWords((*iList)->address->word, (*lista)->address->word)==true){
        (*iList)->next = *lista;
        (*lista)=(*iList);
        (*iList)=nextI;
        nextI=nextI->next;
    }
    struct Parola *curL = *lista, *nextL=(*lista)->next;
    while(nextL && (*iList)){
        if(compareWords((*iList)->address->word, curL->address->word)==false &&
           compareWords((*iList)->address->word, nextL->address->word)==true){
            nextI=(*iList)->next;
            curL->next=(*iList);
            (*iList)->next=nextL;
            curL=(*iList);
            (*iList)=nextI;
        }
        else{
            if(nextL->next){
                nextL= nextL->next;
                curL=curL->next;
            }else {
                nextL->next = (*iList);
                (*iList) = NULL;
                return;
            }
        }
    }
    if(!nextL && (*iList))
        curL->next=(*iList);
}

int main(void){
    char *esclusi=NULL;
    int length, nTry, match=0, goal=0, start=-1, tentativo=0;
    char BUF[10];
    char *end;
    if(fgets(BUF, sizeof(BUF), stdin)){
        BUF[strlen(BUF)-1]=0;
        k=strtol(BUF, &end, 10);}
    if(0<k && k<=17)
        length=19;
    else{
        if(k==0)
            return(0);
        else
            length=k+2;
    }
    char line[length];
    char rif[k+1], p[k+1], r[k+1];
    do{
        if(!fgets(line, length, stdin))
            break;
        if(match ==0){
            if(line[0]== '+' && line[1]== 'n'){
                match=1;
                goal=0;
                start=-1;
            }else{
                if(k>0 && line[0]!='+'){
                    insertRBNode(line);
                }
            }
        }else{
            if(line[0]=='+'){
                if(line[1]=='s'){
                    if(head==NULL || tentativo ==0)
                        inorder(root);
                    else
                        printList(head);
                }
                if(line[1]=='i'){
                    if(fgets(line, length, stdin)){}
                    do{
                        RBNode *w = insertRBNode(line);
                        if(checkConstraints(w->word, esclusi, w)==1 && tentativo>0)
                            insertInSubList(&inserisci, w);
                        if(fgets(line, length, stdin)){}
                    }while(line[0]!='+');
                    mergeLists(&head, &inserisci);
                    inserisci=NULL;
                }
            }else{
                if(start!=1){
                    if(start==0){
                        nTry=strtol(line, &end, 10);
                        start=1;
                    }
                    if(start==-1){
                        memcpy(rif, line, k);
                        rif[k]='\0';
                        start=0;
                    }
                }else{
                    memcpy(p, line, k);
                    p[k]='\0';
                    if(strcmp(rif, p)==0){
                        fprintf(stdout, "ok\n");
                        goal=1;
                        match=0;
                    } else{
                        if(belongsToGlossary(p)==1){
                            if(tentativo<nTry){
                                strcpy(r, rif);
                                tentativo=tentativo+1;
                                for(int m=0; m<k; m++){
                                    int check = checkBelongsToSet(r, p[m]);
                                    if(check==0){
                                        if(esclusi==NULL || strchr(esclusi, p[m])==NULL)
                                            esclusi=insertFirstConstraint(p[m], esclusi, &head);
                                        doesNotBelongToWord(p,m);
                                    }
                                    if(check==1)
                                        belongsToWord(r, p, m, head);
                                    if(tentativo==1 && m==k-1)
                                        createListFromTree(esclusi, root, &head);
                                }
                                fprintf(stdout, "%s\n%d\n", p, N);
                            }
                            if(tentativo==nTry){
                                fprintf(stdout, "ko\n");
                                match=0;
                                goal=1;}
                        }else
                            fprintf(stdout, "not_exists\n");
                    }
                    if(goal==1){
                        while(head!=NULL){
                            struct Parola *tmp=head;
                            head=head->next;
                            free(tmp);}
                        while(h!=NULL)
                            deleteSymbol(&h, h->s);
                        free(esclusi);
                        esclusi=NULL;
                        tentativo=0;
                        N=0;
                    }
                }
            }
        }
    }while(true);
    return 0;
}