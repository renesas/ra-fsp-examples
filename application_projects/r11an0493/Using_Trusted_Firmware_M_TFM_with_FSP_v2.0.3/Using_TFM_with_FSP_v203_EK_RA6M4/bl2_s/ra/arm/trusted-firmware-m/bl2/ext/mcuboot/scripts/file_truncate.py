import sys, getopt

def main(argv):
    input_file      = ''
    truncate_length = 0

    try:
        opts, args = getopt.getopt(argv, "hi:l:",["ifile=","len="])
    except:
        print("file_trucate.py -i <file to truncate> -l <number of bytes to truncate to>")
        sys.exit(2)
    
    for opt, arg in opts:
        if opt == '-h':
            print("file_trucate.py -i <file to truncate> -l <number of bytes to truncate to>")
            sys.exit(0)
        elif opt in ("-i", "--ifile"):
            input_file = arg
        elif opt in ("-l", "--len"):
            truncate_length = int(arg)
    
    print("Truncating: " + input_file + " to max length of " + str(truncate_length) + " bytes")

    try:
        f = open(input_file, "a+")
    except:
        print("Couldn't open file.")
        sys.exit(2)

    f.truncate(truncate_length)
    print("File truncated")

    f.close()

    sys.exit(0)


if __name__ == "__main__":
    main(sys.argv[1:])