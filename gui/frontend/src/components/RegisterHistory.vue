<template>
  <div>
    <div v-for="(registers_content, key) in register_history" :key="key">
      <div v-if="counter === 2 * key">
        <Registers :registers_content="registers_content" />
      </div>
    </div>
  </div>
</template>

<script>
import Registers from "./Registers.vue";

export default {
  name: "RegisterHistory",
  data() {
    return {
      counter: 0,
    };
  },
  props: {
    register_history: Array,
  },
  components: {
    Registers,
  },
  mounted() {
    // Simulating counter increment
    setInterval(() => {
      this.counter++;
    }, 100);
  },
};
</script>
