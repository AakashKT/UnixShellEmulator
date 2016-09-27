
typedef struct node {
	int pid;
	char pname[1000];

	struct node* next;
	struct node* prev;
} list;

extern list* rootNode;
extern char homeDirectory[1000];

void showShellPrompt(char *message);
void changeDir(char *newDir);
void executeCommand(char inp[1000]);
void executeCommandBg(char inp[1000]);
void addToPool(char inp[1000], int pid);
void printPoolProc();
void deleteFromPool(int pid);