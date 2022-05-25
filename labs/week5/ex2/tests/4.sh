cat isi.s tests/m4.s > exe.s
~cs1521/bin/spim -file exe.s | sed -e 1d
