import argparse
import random, sys

class shuf:
    def __init__(self, filename, input_range, head_count, repeat):
        self.filename = filename
        self.input_range = input_range
        self.head_count = head_count
        self.repeat = repeat

        # exit if file and input range both specified
        if (self.filename or not sys.stdin.isatty()) and self.input_range:
            sys.exit("Error: file input and input range option cannot both be provided.")

        if self.head_count:
        	if int(self.head_count) < 0:
        		sys.exit("Error: invalid line count.")

        # declare and initialize self.lines
        if self.input_range:
            self.lines = []
        elif self.filename == None or self.filename == '-':
            self.lines = sys.stdin.readlines()
        else:
            f = open(filename, 'r')
            self.lines = f.readlines()
            f.close()

    # shuffling and print input
    def shuffle(self):

        # if input range provided, check for proper format
        if self.input_range:
            self.input_range = self.input_range.split('-', 1)

            # only one dash allowed
            if len(self.input_range) != 2:
                sys.exit("Error: invalid range.")

            # will exit if contents are empty or not numbers
            for num in self.input_range:
                if num.isdigit() == False:
                    sys.exit("Error: invalid range provided.")

            # set upper and lower range of input range
            lower = int(self.input_range[0])
            upper = int(self.input_range[1])

            if lower > upper:
                sys.exit("Error: input range cannot range from a larger to smaller number.")

            # add numbers in the input range to self.lines, followed by a new line
            for i in range(lower, upper+1):
                self.lines.append(str(i) + "\n")

        while True:
            # shuffle contents of self.lines
            random.shuffle(self.lines)

            inputLength = len(self.lines)

            # set repeat count to number of lines in input or to head count
            numLines = inputLength
            if self.head_count:
                if numLines > int(self.head_count) or self.repeat:
                    numLines = int(self.head_count)

            # print out all randomized values
            for i in range(numLines):
                sys.stdout.write(self.lines[i % inputLength])

                # if head count and repeat options were specified, then shuffle self.lines after each print to replicate random sampling with replacement
                if self.head_count and self.repeat:
                    random.shuffle(self.lines)

            # break if head count specified or repeat is not True
            if self.repeat != True or self.head_count:
                break

def main():
    parser = argparse.ArgumentParser(description="Print to standard output a random permutation of the input. With no FILE, or when FILE is '-', read from standard input.")

    # allowable arguments and options
    parser.add_argument("FILE", nargs='?', default=None, help="File to be permuted. Providing a dash or empty input will take input from standard input.")
    parser.add_argument("-i", "--input-range", default=None, help="Treat each number in [LO, HI] as input.")
    parser.add_argument("-n", "--head-count", default=None, help="Limit the maximum number of lines returned.")
    parser.add_argument("-r", "--repeat", action="store_true", default=None, help="Run indefinitely, or until the number of lines specified with head count is reached.")
    args = parser.parse_args()

    try:
        generator = shuf(args.FILE, args.input_range, args.head_count, args.repeat)
    except Exception as e:
        parser.error(e)

    # shuffle and print out input
    generator.shuffle()

# call main function
if __name__ == '__main__':
    main()
