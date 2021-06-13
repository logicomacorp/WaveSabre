#if defined(_MSC_VER)
int __getmainargs(int *argc, char ***argv, char ***env, int dowildcards, int *new_mode);
int main(int argc, char **argv);
int mainCRTStartup()
{
	int argc;
	char **argv, **env;
	int new_mode = 0;
	__getmainargs(&argc, &argv, &env, 0, &new_mode);
	return main(argc, argv);
}
#endif