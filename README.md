# CPP-Bptree
Insert implementation:
Checks if leaf exist or if key is less than the lowest key then shifts the keys and places the lowest key at leaf->key[0]
Else the key proceeds to find a leaf that has keys that are greater than or equal to that key. It then shifts the keys if need be and places itself to the right of the key that it is greater than or equal to. 

Split implementation:
Create a Temp node that can take 1 more value than the node and put the new key and pointer within the temp alongside with the copy of the full node. Then we split the temp into two parts and set them as leaves. We then create a parent node where it contains the value of the part with the larger values and put the smallest of the larger values within the parent node and setting the pointer of N to the smaller half and N + 1 to the larger half. 

Internal Split implementation:
Similar to the split implementation but when the parent node gets filled the first valur of the larger half of the parent node will become the root node. 
