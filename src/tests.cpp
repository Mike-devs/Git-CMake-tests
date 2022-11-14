void GeneratePocoUuid(int argc, char** argv);
void mongo_hello (int argc, char *argv[]);

int main(int argc, char** argv)
{
	GeneratePocoUuid(argc, argv);
	mongo_hello(argc, argv);

	return 0;
}
