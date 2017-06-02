# /*******************************************************************************
#  * Name            : batch_cpplint.py
#  * Project         : FlashPhoto
#  * Module          : cpplint
#  * Description     : Runs the Google C++ Style Guide compliance checker on all
#  *                   .h and .cc files found in the 'include' and 'src' directories,
#  *                   or only those specified in an input include list text file.
#  * Copyright       : 2016 CSCI3081W Group C03. All rights reserved.
#  * Creation Date   : 10/14/16
#  * Original Author : Erik Husby
#  *
#  ******************************************************************************/

import sys
import argparse
import os
import os.path
import re
from subprocess import call


def main():
    # Set up arguments.
    parser = argparse.ArgumentParser(
        description="Runs the Google C++ Style Guide compliance checker on all \
.h and .cc files found in the 'include' and 'src' directories under the specified \
project iteration folder, or only those specified in an input include list text file")

    # Default directory argument should work when the script is called from
    # within the cpplint directory.
    parser.add_argument('iterationX_dir', nargs='?', default='../..',
                        help="Directory of the iteration you want to check.")
    parser.add_argument('--include_list',
                        help="Text file containing the names of files to check for \
Google C++ Style Guide compliance, relative to iterationX_dir.")

    args = parser.parse_args()

    iterationXDir = os.path.abspath(args.iterationX_dir)
    if not os.path.isdir(iterationXDir):
        parser.error("iterationX_dir '{}' does not exist".format(iterationXDir))

    cpplint = os.path.join(iterationXDir, 'ext', 'cpplint', 'cpplint.py')
    if not os.path.isfile(cpplint):
        print "\nERROR: Cannot find cpplint.py"
        print "Exiting"
        sys.exit()

    if args.include_list:
        includeFile = args.include_list
        if not os.path.isfile(includeFile):
            parser.error("include_list file '{}' does not exist".format(includeFile))

        rel_files_to_check = []
        iFile = open(includeFile, 'r')
        line = iFile.readline().strip()
        while line != '':
            rel_files_to_check.append(line)
            line = iFile.readline().strip()
        iFile.close()

    else:
        sourceDir  = os.path.join(iterationXDir, 'src')
        includeDir = os.path.join(sourceDir, 'include')

        re_include = re.compile(".*.\h$")
        re_source  = re.compile(".*.\cc$")

        includeFiles = [os.path.join(dp, f)
                        for dp, dn, fn in os.walk(os.path.expanduser(includeDir))
                        for f in fn
                        for m in [re_include.match(f)] if m]
        sourceFiles  = [os.path.join(dp, f)
                        for dp, dn, fn in os.walk(os.path.expanduser(sourceDir))
                        for f in fn
                        for m in [re_source.match(f)] if m]

        rel_files_to_check = includeFiles + sourceFiles


    # Call cpplint on all files that need to be checked.
    print "\n\
----------------------\n\
--- STARTING CHECK ---\n\
----------------------\n"
    for file in rel_files_to_check:
        fileName = os.path.join(iterationXDir, file)
        if not os.path.isfile(fileName):
            print "\nERROR: Cannoy find a file specified in include list: {}".format(
                file)
            sys.exit()

        print "\n--- CHECKING FILE: {} ---".format(file)
        call(['python', cpplint, os.path.join(iterationXDir, file)])

    print "\n--- DONE CHECKING ---"
    sys.exit()



if __name__ == '__main__':
    main()
