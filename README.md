# CPP-Bptree
Insert implementation:
Checks if leaf exist or if key is less than the lowest key then shifts the keys and places the lowest key at ;eaf->key[0]
Else the key proceeds to find a leaf that has keys that are greater than or equal to that key. It then shifts the keys if need be and places itself to the right of the key that it is greater than or equal to. 
