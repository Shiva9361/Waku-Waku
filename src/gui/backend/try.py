import processor as p
latencies = {
    "add": 1,
    "addi": 2,
    "sub": 3,
    "mul": 3,
    "div": 4
}

processor = p.Processor("codes/selection_sort.s",
                        "codes/bubble_sort.s", True, True, latencies)
