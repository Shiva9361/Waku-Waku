<template>
  <div>
    <i @click="play_pause" class="fa fa-play" aria-hidden="true"></i>
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
      play: false,
    };
  },
  props: {
    register_history: Array,
  },
  components: {
    Registers,
  },
  methods: {
    play_pause() {
      this.play = !this.play;
    },
  },
  mounted() {
    // Simulating counter increment
    setInterval(() => {
      if (this.play) this.counter++;
    }, 1000);
  },
};
</script>

<style>
.fa {
  color: green;
}
</style>
