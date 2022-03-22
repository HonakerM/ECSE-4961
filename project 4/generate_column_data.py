import random
import string
import sys

if len(sys.argv) != 6:
    print("format is: python3 {} <out_file> <num_of_keys> <num_of_entries> <min_key_size> <max_key_size>".format(sys.argv[0]))
    exit(1)

random.seed(0)

num_of_keys = int(sys.argv[2])
num_of_entries = int(sys.argv[3])
min_key_size = int(sys.argv[4])
max_key_size = int(sys.argv[5])
file_name = sys.argv[1]

print("generating key list")

key_list = []
for i in range(num_of_keys):
    random_number = random.randrange(min_key_size,max_key_size)
    new_key = ""
    for j in range(random_number):
        new_key += random.choice(string.ascii_letters)
    new_key += '\n'
    key_list.append(new_key)


print("generting output file")

file = open(file_name, 'w')
for i in range(num_of_entries):
    key_loc = random.randrange(0, num_of_keys)
    file.writelines([key_list[key_loc]])

file.close()