parse_table = {
    "S": {
        "a": ["A", "B", "B", "C"]
    },
    "B": {
        "b": ["b", "E"]
    },
    "E": {
        "a": ["A"],
        "b": ["b"]
    },
    "C": {
        "a": ["a"],
        "c": ["c"]
    },
    "A": {
        "a": ["a", "F"],
    },
    "F": {
        "a": ["a"],
    },
}
class LL1Parser:
    def __init__(self, parse_table, start_symbol, end_symbol):
        self.parse_table = parse_table
        self.start_symbol = start_symbol
        self.end_symbol = end_symbol

    def parse(self, input_string):
        input_queue = list(input_string) + [self.end_symbol]
        parse_stack = [self.end_symbol, self.start_symbol]

        while parse_stack:
            top = parse_stack.pop()
            current_input = input_queue[0]
            if top == current_input:
                input_queue.pop(0)
            elif top in self.parse_table:
                if current_input in self.parse_table[top]:
                    rule = self.parse_table[top][current_input]
                    if rule != 'ε':
                        parse_stack.extend(reversed(rule))
                    print(f"{top} → {''.join(rule)}")
                else:
                    raise SyntaxError(
                        f"Unexpected symbol: {current_input}.\nSentence {input_string} does not belong to this grammar.")
            else:
                raise SyntaxError(
                    f"Unexpected symbol: {top}.\nSentence {input_string} does not belong to this grammar.")

        if input_queue and input_queue[0] != self.end_symbol:
            raise SyntaxError(
                f"Parsing did not complete properly.\nSentence {input_string} does not belong to this grammar.")

        print(f"Parsing completed successfully.\nSentence {input_string} belongs to this grammar.")


def main():
    print("LL(1) analyzer for CS grammar")

    parser = LL1Parser(parse_table, "S", "$")

    sentence: str = input("Enter your sentence: ")

    try:
        parser.parse(sentence)
    except SyntaxError as e:
        print(e)


if __name__ == "__main__":
    main()