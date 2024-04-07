<template>
  <div>
    <div v-for="(value, key) in memory" :key="key">
      <div class="mem_key">{{ key }}</div>
      <div class="mem_value">{{ value }}</div>
    </div>
  </div>
</template>

<script>
export default {
  name: "MemoryComponent",
  props: {
    memory: {
      type: Object,
      required: true,
    },
  },
  mounted(){
    console.log(this.memory)
  }
};
</script>

<style>
.mem_key {
  margin: 1px 1px 0px 0px;
  border-style: solid;
  display: inline-block;
  width: 70px;
}
.mem_value {
  border-style: solid;
  display: inline-block;
  width: 300px;
}
</style>
