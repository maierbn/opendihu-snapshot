

for i in `seq 1 5`; do
  aprun -n 24 ./shorten_implicit ../settings_shorten_hazelhen.py scenario-name
done

 
