<template></template>

<script>
export default {
  name: "App",
  data() {
    return {
      pipeline_instructions: Array,
    };
  },
  async created() {
    const res = await fetch("http://localhost:5000/core/0/pipe");
    const instructions = await res.json();
    this.pipeline_instructions = instructions;
    console.log(pipeline_instructions);
  },
};
</script>

<style></style>
