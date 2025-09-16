if get_option('with_test').enabled()
    run_command(['python3', 'tools' / 'runner.py'], check: true)
    cards = run_command(['python3', 'tools' / 'wildcard.py'], check: true)

    test_cases = ['unit_runner.c', 'minimal.elf', cards.stdout().strip().split('\n')]

    pizza_c = executable('pizza', test_cases, include_directories: dir,
        dependencies: [dependency('fossil-test'), fossil_media_dep])

    test('fossil testing C', pizza_c)
endif
