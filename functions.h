
extern char homeDirectory[1000];

void showShellPrompt(char *message);
void changeDir(char *newDir);
void executeCommand(char *command, char *argv[]);
void executeCommandBg(char *command, char *argv[]);