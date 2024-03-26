def parse(str: str):
    adjacency = {
        1 : {"b" : 3, "a" : 2 },
        2 : {"b" : 4, "c" : 4 },
        3 : {"c" : 5},
        4 : {},
        5 : {"c" : 5}
    }
    final_states = [1,3,4,5]

    current_state = 1

    for sym in str:
        if sym in adjacency[current_state]:
            current_state = adjacency[current_state][sym]
        else:
            return False
    if current_state in final_states:
        return True
    else: return False

if __name__ == "__main__":
    # True
    print(parse("ab"))
    print(parse("bccc"))
    print(parse("b"))
    print(parse(""))
    print(parse("ac"))
    # False
    print(parse("abb"))
    print(parse("acc"))
    print(parse("abc"))