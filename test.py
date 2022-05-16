
delete_rows = [[1, 2, 3], [1, 2, 4], [1, 2, 5]]
flag = False

# 二维数组仅保留重复组
for i in range(len(delete_rows)):
    for j in range(len(delete_rows[0])):
        flag = False
        for k in range(len(delete_rows[i])):
            if delete_rows[0][j] == delete_rows[i][k]:
                flag = False
                break
            else:
                flag = True
    if flag:
        delete_rows[0].remove(delete_rows[0][j])
print (delete_rows)
