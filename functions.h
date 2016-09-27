
extern char homeDirectory[1000];

void showShellPrompt(char *message);
void changeDir(char *newDir);
void executeCommand(char inp[1000]);
void executeCommandBg(char inp[1000]);