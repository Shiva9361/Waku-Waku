<template>
  <div class="pipeline">
    <div v-for="(value, key) in pipeline_content" :key="key">
      <div>{{ value }}</div>
    </div>
  </div>
</template>

<script>
export default {
  name: "PipelineComponent",
  props: {
    pipeline_content: Object,
  },
};
</script>

<style>

</style>
