f = open("DrawBarycentric.txt", "r")
f1 = f.read().split("\n") ## Split by newline


f = open("DrawHalfPlane.txt", "r")
f2 = f.read().split("\n") ## Split by newline


# Get co-ordinates in f1 but not in f2
s = set(f2)
diff = "\n".join([x for x in f1 if x not in s])
print("Co-ordiantes present in Barycentric but not in Half Plane: \n" + diff + "\n\n")

# Get co-ordinates in f2 but not in f1
s = set(f1)
diff = "\n".join([x for x in f2 if x not in s])
print("Co-ordiantes present in Half Plane but not in Barycentric: \n" + diff + "\n\n")
