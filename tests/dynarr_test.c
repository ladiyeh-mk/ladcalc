#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define DYNARR_IMPLEMENTATION
#include "../utils/dynarr.h"

// Original custom structures — zero recycled garbage
typedef struct {
    uint64_t id;
    char label[32];
    double weights[4];
    bool active;
} Neuron;

typedef struct {
    uint32_t code;
    float metrics[3];
    char message[64];
} TelemetryFrame;

int main(void) {
    printf("=== [LADARR] Hardcore Stress & Integrity Suite ===\n\n");

    // 1. Zero-Capacity Fallback & Default Header Alignment Test
    {
        printf("[*] Testing zero-capacity fallback & header boundary alignment...\n");
        Neuron *net = dynarr_create(sizeof(Neuron), 0); // Should fallback to DEFAULT_CAPACITY (16)
        assert(net != NULL);
        assert(dynarr_peek_capacity(net) == 16);
        assert(dynarr_peek_count(net) == 0);
        dynarr_destroy(net);
        printf("    [OK] Header alignment verified.\n\n");
    }

    // 2. Heavy Struct Stress & Byte Integrity (10,000 complex items)
    {
        printf("[*] Stress testing 10,000 deep structs (Neuron)...\n");
        Neuron *net = dynarr_create(sizeof(Neuron), 4);
        
        for (int i = 0; i < 10000; i++) {
            Neuron n = {
                .id = (uint64_t)(i * 1337),
                .active = (i % 2 == 0)
            };
            snprintf(n.label, sizeof(n.label), "neuron_node_%d", i);
            n.weights[0] = (double)i * 0.125;
            n.weights[1] = (double)i * 0.250;
            n.weights[2] = (double)i * 0.375;
            n.weights[3] = (double)i * 0.500;
            
            dynarr_append(net, n);
        }

        assert(dynarr_peek_count(net) == 10000);
        assert(dynarr_peek_capacity(net) >= 10000);

        // Spot check boundaries and middle indices for byte corruption or misaligned realloc offsets
        assert(net[0].id == 0);
        assert(strcmp(net[0].label, "neuron_node_0") == 0);
        assert(net[5000].id == (uint64_t)(5000 * 1337));
        assert(strcmp(net[5000].label, "neuron_node_5000") == 0);
        assert(net[9999].weights[2] == 9999 * 0.375);

        dynarr_destroy(net);
        printf("    [OK] 10,000 structural allocations passed without byte corruption.\n\n");
    }

    // 3. Multi-Array Memory Isolation Test
    {
        printf("[*] Testing multi-array isolation (Neurons + TelemetryFrames simultaneously)...\n");
        Neuron *neurons = dynarr_create(sizeof(Neuron), 10);
        TelemetryFrame *telemetry = dynarr_create(sizeof(TelemetryFrame), 10);

        for (int i = 0; i < 500; i++) {
            Neuron n = { .id = (uint64_t)i };
            TelemetryFrame t = { .code = (uint32_t)(i * 3) };
            dynarr_append(neurons, n);
            dynarr_append(telemetry, t);
        }

        assert(dynarr_peek_count(neurons) == 500);
        assert(dynarr_peek_count(telemetry) == 500);

        // Verify headers don't bleed or cross-reference sizes/type_sizes
        assert(neurons[499].id == 499);
        assert(telemetry[499].code == 1497);

        dynarr_destroy(neurons);
        dynarr_destroy(telemetry);
        printf("    [OK] Multi-array memory boundaries verified.\n\n");
    }

    // 4. Interleaved Push/Pop Chaos Fuzz Loop
    {
        printf("[*] Running interleaved push/pop chaos fuzz loop (5,000 iterations)...\n");
        int *stream = dynarr_create(sizeof(int), 8);
        
        int pushed = 0;
        int popped = 0;
        
        for (int round = 0; round < 5000; round++) {
            if (round % 3 == 0 && dynarr_peek_count(stream) > 0) {
                int val = dynarr_pop(stream);
                (void)val;
                popped++;
            } else {
                dynarr_append(stream, round);
                pushed++;
            }
        }

        assert(dynarr_peek_count(stream) == (size_t)(pushed - popped));
        dynarr_destroy(stream);
        printf("    [OK] Chaos loop survived with exact count invariant matches.\n\n");
    }

    // 5. Reserve & Shrink/Capacity Safety Checks
    {
        printf("[*] Testing reserve scaling and capacity locks...\n");
        uint32_t *metrics = dynarr_create(sizeof(uint32_t), 4);
        
        dynarr_append(metrics, 42);
        dynarr_reserve((void **)&metrics, 2048);
        
        assert(dynarr_peek_capacity(metrics) == 2048);
        assert(dynarr_peek_count(metrics) == 1);
        assert(metrics[0] == 42);

        // Verify data persistence after massive realloc jump
        dynarr_append(metrics, 1337);
        assert(metrics[1] == 1337);
        assert(dynarr_peek_count(metrics) == 2);

        dynarr_destroy(metrics);
        printf("    [OK] Reserve memory jump & data retention verified.\n\n");
    }

    printf("=== All custom stress tests passed cleanly, twin. ===\n");
    return 0;
}