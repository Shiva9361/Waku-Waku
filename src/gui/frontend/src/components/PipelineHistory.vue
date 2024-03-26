<template>
  <div class="pipecontainer">
    <h4 style="margin: 5px 0px 5px 0px">Core: {{ core_number }}</h4>
    <div class="clk" v-if="is_runnning">Clock: {{ counter + 1 }}</div>
    <div class="clk" v-if="!is_runnning">Clock: {{ counter }}</div>
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
    core_number: Number,
    is_runnning: Boolean,
  },
  components: {
    Pipeline,
  },
};
</script>
<style>
.pipecontainer {
  width: 525px;
  margin: 20px auto;
  overflow: auto;
  height: 100px;
  border: 1px solid steelblue;
  padding: 10px;
  border-radius: 5px;
}
</style>
