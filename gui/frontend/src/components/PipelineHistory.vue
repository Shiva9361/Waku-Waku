<template>
  <div>
    <Pipeline :pipeline_content="pipeline_history[counter]" />
  </div>
</template>

<script>
import Pipeline from "./Pipeline.vue";

export default {
  name: "PipelineHistory",
  data() {
    return {
      counter: 0,
    };
  },
  props: {
    pipeline_history: Array,
  },
  components: {
    Pipeline,
  },
  mounted() {
    // Simulating counter increment
    setInterval(() => {
      this.counter++;
    }, 1000);
  },
};
</script>
