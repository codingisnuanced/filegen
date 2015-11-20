#Simple filegen
Generates HTML pages from files in a single directory.

###Options

*-v* verbose

*-r* randomize files

*-d* directory with files to generate from. Uses audio/ by default

*-f* number of files to generate from

*-n* number of folders to create to hold HTML files

*-p* number of HTML files per generated folder

### Generated Files and Folders

A folder named *html* in the working directory. This will contain a specified number of randomly named folders (-n) with the specified number of HTML files (-p) in each.

A document named *html_dirs.txt* in the working directory. This will contain a list of the relative paths to the randomly named directories.

A document named *html_files.txt* in the working directory. This will contain a list of relative paths to the individual html files.

An HTML file named *index.html* in the working directory. This webpage will contain a list of anchor tags linking to the rest of the generated webpages.

###Usage

*make ftoh*

*./ftoh -rv -d audio -f 3500 -n 35 -p 100*

####Thank You

Look at the *Makefile* for more rules including *make nginx* which will copy the generated folders and files to folder /usr/share/nginx/filegen/

