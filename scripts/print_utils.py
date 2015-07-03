class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def print_bold(message):
    print(bcolors.BOLD + message + bcolors.ENDC);

def print_header(message):
    print(bcolors.HEADER + message + bcolors.ENDC);

def print_warning(message):
    print(bcolors.WARNING + message + bcolors.ENDC);

def print_green(message):
    print(bcolors.OKGREEN + message + bcolors.ENDC);

def print_blue(message):
    print(bcolors.OKBLUE + message + bcolors.ENDC);

def print_red(message):
    print(bcolors.FAIL + message + bcolors.ENDC);
