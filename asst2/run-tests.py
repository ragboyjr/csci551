import re
import sys

test_str = """Enter a, b, and n
100 
600 
1000000 

Running on 10 processors.
Elapsed time = 5.884409e-02 seconds 
With n = 1000000 trapezoids, 
our estimate of the integral from 100.000000 to 600.000000 = 4.0037209001535e+03 
absolute relative true error = 5.539354e-13 is NOT less than criteria = 5.000000e-13"""

float_e_pattern  = "(\d+.\d+e(-|\+)?\d+)"
anything_pattern = "[.\s]+"

output_re_pattern = \
    r"Running on (\d+) processors%(any)s" + \
    r"Elapsed time = %(float)s seconds\n(With) %(any)s"# + \
#    r"With"
#    r"With n = (\d+)%(any)s" + \
#    r"our estimate.+from (\d+\.\d+) to (\d+\.\d+) = %(float)s"

output_re_pattern = output_re_pattern % \
                    {"float": float_e_pattern, "any": anything_pattern}

print output_re_pattern

output_re = re.compile(output_re_pattern, re.M);
m = output_re.search(test_str)

if m:
    print m.groups()
else:
    print "No matches found"
