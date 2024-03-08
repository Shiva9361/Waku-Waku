<template>
  <div class="container">
    <Pipeline :pipeline_content="pipeline_history[counter]" />
  </div>
</template>

<script>
import Pipeline from "./Pipeline.vue";

export default {
  name: "PipelineHistory",
  props: {
    pipeline_history: Array,
    counter: Number,
  },
  components: {
    Pipeline,
  },
};
</script>
