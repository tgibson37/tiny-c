# tests an app via the test system, see tests 73,74 for example
# requires input and expected output files
# arg $1 is the name of the SampleProgram/$1.tc
# The results of these tests are in diff_$1.txt and must be
#	examined separately from test/dump_results using...
#			   make diffp
echo test_$1 >diff_$1.txt
./tc Sa*/$1.tc <testFiles/input_$1.txt >output_$1.txt
diff testFiles/expected_$1.txt output_$1.txt >>diff_$1.txt
echo test_$1 done >>diff_$1.txt
cat diff_$1.txt
