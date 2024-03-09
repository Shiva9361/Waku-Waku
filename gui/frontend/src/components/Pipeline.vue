<template>
  <div class="pipeline">
    <div v-for="(value, key) in pipeline_content" :key="key">
      <div class="inline">
        <div v-if="key == 0">WB:&nbsp;</div>
        <div v-if="key == 1">MEM:&nbsp;</div>
        <div v-if="key == 2">EX:&nbsp;</div>
        <div v-if="key == 3">ID/RF:&nbsp;</div>
        <div v-if="key == 4">IF:&nbsp;</div>
        <div>{{ value }}&nbsp;&nbsp;</div>
      </div>
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
.pipeline {
  width: 100%;

  display: flex;
}
.inline {
  display: flex;
}
</style>
