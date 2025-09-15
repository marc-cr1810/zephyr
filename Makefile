all:
	cmake -S . -B build
	cmake --build build

clean:
	rm -rf build

# Embedding examples
examples:
	cmake -S . -B build -DBUILD_EMBEDDING_EXAMPLE=ON
	cmake --build build

comprehensive:
	cmake -S . -B build -DBUILD_EMBEDDING_EXAMPLE=ON
	cmake --build build --target comprehensive_embedded_example

run-comprehensive:
	cmake -S . -B build -DBUILD_EMBEDDING_EXAMPLE=ON
	cmake --build build --target comprehensive_embedded_example
	./bin/comprehensive_embedded_example

run-zephyr-demo:
	cmake -S . -B build
	cmake --build build
	./bin/zephyr examples/comprehensive_embedded_demo.zephyr
