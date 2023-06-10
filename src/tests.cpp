void GeneratePocoUuid(int argc, char** argv);
void mongo_hello (int argc, char *argv[]);
void LaunchTimeServer(int argc, char** argv);

int main(int argc, char** argv)
{
	GeneratePocoUuid(argc, argv);
	//mongo_hello(argc, argv);
	LaunchTimeServer(argc, argv);

	return 0;
}
