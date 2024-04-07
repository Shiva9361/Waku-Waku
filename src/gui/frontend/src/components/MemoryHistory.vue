<template>
  <div>
    <Memory :memory="memory_history[mem_counter]" />

  </div>
</template>

<script>
import Memory from "./Memory.vue";

export default {
  name: "MemoryHistory",
  props: {
    memory_history: Array,
    mem_counter: Number,
  },
  components: {
    Memory,
  },
};
</script>

<style></style>
