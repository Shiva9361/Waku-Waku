<template>
  <div>
    <Registers :registers_content="register_history[counter]" />
  </div>
</template>

<script>
import Registers from "./Registers.vue";

export default {
  name: "RegisterHistory",
  data() {
    return {
      counter: 0,
      regitser_hist: [],
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
    }, 1000);
  },
};
</script>
