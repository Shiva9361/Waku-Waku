<template>
  <div>
    <Registers :registers_content="register_history[counter]" />
  </div>
</template>

<script>
import Registers from "./Registers.vue";

export default {
  name: "RegisterHistory",
  props: {
    register_history: Array,
    counter: Number,
  },
  components: {
    Registers,
  },
};
</script>

<style>
.fa {
  color: green;
}
</style>
