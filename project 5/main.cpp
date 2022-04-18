
#include "main.h"

extern struct fuse_operations bb_oper;


std::unordered_map<key_type, value_type> enc_table;
std::unordered_map<value_type, key_type> dec_table;


void bb_usage()
{
    fprintf(stderr, "usage:  main.o [FUSE and mount options] rootDir mountPoint encTable\n");
    abort();
}



void read_enc_table(std::string filename) {
    std::ifstream enc_file(filename);

    std::string line;
    value_type line_count = 0;
    while (std::getline(enc_file, line)) {
        enc_table.insert(std::make_pair(line, line_count));
        dec_table.insert(std::make_pair(line_count, line));
    }
}



int main(int argc, char *argv[])
{
    int fuse_stat;
    struct bb_state *bb_data;

    // bbfs doesn't do any access checking on its own (the comment
    // blocks in fuse.h mention some of the functions that need
    // accesses checked -- but note there are other functions, like
    // chown(), that also need checking!).  Since running bbfs as root
    // will therefore open Metrodome-sized holes in the system
    // security, we'll check if root is trying to mount the filesystem
    // and refuse if it is.  The somewhat smaller hole of an ordinary
    // user doing it with the allow_other flag is still there because
    // I don't want to parse the options string.
    /*if ((getuid() == 0) || (geteuid() == 0)) {
    	fprintf(stderr, "Running BBFS as root opens unnacceptable security holes\n");
    	return 1;
    }*/

    // See which version of fuse we're running
    fprintf(stderr, "Fuse library version %d.%d\n", FUSE_MAJOR_VERSION, FUSE_MINOR_VERSION);
    
    // Perform some sanity checking on the command line:  make sure
    // there are enough arguments, and that neither of the last two
    // start with a hyphen (this will break if you actually have a
    // rootpoint or mountpoint whose name starts with a hyphen, but so
    // will a zillion other programs)
    if ((argc < 4) || (argv[argc-2][0] == '-') || (argv[argc-1][0] == '-'))
	bb_usage();

    bb_data = (bb_state*)malloc(sizeof(struct bb_state));
    if (bb_data == NULL) {
	perror("main calloc");
	abort();
    }

    // Pull the rootdir out of the argument list and save it in my
    // internal data
    //bb_data->rootdir = realpath(argv[argc-2], NULL);
    read_enc_table(argv[argc-1]);
    argv[argc-1] = NULL;
    argc--;

    
    bb_data->rootdir = realpath(argv[argc-2], NULL);
    argv[argc-2] = argv[argc-1];
    argv[argc-1] = NULL;
    argc--;
    
    bb_data->logfile = log_open();
    
    // turn over control to fuse
    fprintf(stderr, "about to call fuse_main\n");
    fuse_stat = fuse_main(argc, argv, &bb_oper, bb_data);
    fprintf(stderr, "fuse_main returned %d\n", fuse_stat);
    
    return fuse_stat;
}
